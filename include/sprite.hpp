#ifndef _SPRITE_HPP_
#define _SPRITE_HPP_

#include "node.hpp"
#include "texture.hpp"
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
            boost::shared_ptr<Texture> atexture;
        public:
            Animation() :
                atime(0)
            {
            }

			Animation(cmn::uint animation_time) :
				atime(animation_time)
			{}

            Animation(cmn::uint animation_time,
                      const boost::shared_ptr<Texture>& animation_texture) :
                atime(animation_time),
                atexture(animation_texture)
            {
            }

            cmn::uint duration() const
            {
                return atime;
            }

            const boost::shared_ptr<Texture>& texture() const
            {
                return atexture;
            }

			void set_texture(boost::shared_ptr<Texture> t)
			{
				atexture = t;
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

			size_t get_frames_number() const
			{
				return animations_.size();
			}
    };

    class Sprite : public Node
    {
	private:
		typedef std::map <cmn::uint, AnimationList> almap;
		almap al_;
		bool is_alive_;
		bool is_running_;
		float x_size_, y_size_;
		bool reset_position_;
		v3d pos_;   // need while we use reset position
		AnimationList* current_al_;
		Animation cur_animation;
	private:
		// implementations
		void draw_impl(boost::shared_ptr<Driver> driver);
		void update_impl(cmn::uint tick);
		void set_position(const v3d& pos);
		matrixf get_matrix() const;
		void start_impl(cmn::uint)
		{
			execute(0);
		}

		Node* clone_impl() const;
	public:
		Sprite() : is_alive_(true), is_running_(false), x_size_(0.0), y_size_(0.0),
				   current_al_(0)
		{}
		Sprite(const AnimationList& al) : is_alive_(true), is_running_(false), x_size_(0.0), y_size_(0.0),
										  current_al_(0)
		{
			al_[al.getIndex()] = al;
		}

		Sprite(const Sprite& sprite);

		void addAnimationList(const AnimationList& al);

		const AnimationList* get_current_animation_list() const
		{
			return current_al_;
		}

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

		size_t get_frames_number() const
		{
			if (current_al_) return current_al_->get_frames_number();
			return 0;
		}	

		size_t get_animations_number() const
		{
			return al_.size();
		}				

		// execute animation from list with index <index>
		void execute(cmn::uint index);
    };
}

#endif
