#ifndef _SPRITE_HPP_
#define _SPRITE_HPP_

#include "inode.hpp"
#include "itexture.hpp"
#include "mhe_math.hpp"
#include "transform.hpp"
#include <vector>
#include <map>

namespace mhe
{
    class Animation : public Transform
    {
        private:
            cmn::uint atime;    // time of playing current animation
            boost::shared_ptr<iTexture> atexture;
        public:
            Animation() :
                atime(0)
            {
            }

            Animation(cmn::uint animation_time,
                      const boost::shared_ptr<iTexture>& animation_texture) :
                atime(animation_time),
                atexture(animation_texture)
            {
            }

            cmn::uint duration() const
            {
                return atime;
            }

            const boost::shared_ptr<iTexture>& texture() const
            {
                return atexture;
            }
    };

    class AnimationList
    {
        public:
            enum AnimationChangeType
            {
                FirstAnimation,
                NextAnimation,
                LastAnimation,
                NoChange
            };
        private:
            std::vector<Animation> animations_;
            bool autorepeat_;
            cmn::uint next_tick_;
            size_t cur_animation_;
            cmn::uint index_;
        public:
            AnimationList(bool repeat = false);

            void reset();
            void add(const Animation& a);
            void remove(cmn::uint begin, cmn::uint number);
            void start(cmn::uint tick);
            void stop();
            bool next();

            AnimationChangeType update(cmn::uint tick);

            bool get(Animation& a) const;
            bool get(cmn::uint tick, Animation& a);

            bool is_running() const
            {
                return next_tick_;
            }

            // index getter/setter
            void setIndex(cmn::uint index)
            {
                index_ = index;
            }

            cmn::uint getIndex() const
            {
                return index_;
            }
    };

    class Sprite : public iNode
    {
        private:
            typedef std::map <cmn::uint, AnimationList> almap;
            almap al_;
            bool is_alive_;
            float x_size_, y_size_;
            bool reset_position_;
            v3d pos_;   // need while we use reset position
            AnimationList* current_al_;
			Animation cur_animation;
        private:
            // implementations
            void draw_impl(const boost::shared_ptr<iDriver>& driver);
            void update_impl(cmn::uint tick);
            void set_position(const v3d& pos);
            matrixf get_matrix() const;
        public:
            Sprite() : is_alive_(true), x_size_(0.0), y_size_(0.0), reset_position_(true),
				current_al_(0)
			{}
            Sprite(const AnimationList& al) : is_alive_(true), x_size_(0.0), y_size_(0.0),
                reset_position_(false), current_al_(0)
            {
                al_[al.getIndex()] = al;
            }

            Sprite(const AnimationList& al, const v3d& position);

            void addAnimationList(const AnimationList& al);
			void setSize(float size)
			{
				x_size_ = y_size_ = size;
			}

			void setSize(float xsize, float ysize)
			{
				x_size_ = xsize;
				y_size_ = ysize;
			}

			float width() const;
			float height() const;

			void setPosition(const v3d& pos)
			{
				set_position(pos);
			}

            // execute animation from list with index <index>
            void execute(cmn::uint index, cmn::uint tick, bool reset_position = false);
    };
}

#endif
