#ifndef _PARTICLE_HPP_
#define _PARTICLE_HPP_

#include "inode.hpp"
#include "mhe_math.hpp"
#include "idriver.hpp"
#include <boost/array.hpp>

namespace mhe
{
    const size_t particle_vertexes_num = 12;
    const size_t particle_indices_num  = 6;
	class Particle : public iNode
	{
        public:
            typedef boost::array<float, particle_vertexes_num> varray;
            typedef boost::array<cmn::uint, particle_indices_num> iarray;
		private:
			v3d pos_;
			v3d vel_;	// velocity
			cmn::uint lifetime_;
			colorf begin_color_;
			colorf end_color_;
			colorf cur_color_;
			bool is_alive_;
			cmn::uint start_time_;
			cmn::uint before_die_;
			float size_;
            varray v;
            varray n;
            iarray i;
		private:
			void draw_impl(const boost::shared_ptr<iDriver>& driver);
			void update_impl(cmn::uint tick);
		public:
			Particle();
			Particle(const Particle&);

			void start(cmn::uint tick);
			void kill();

			const v3d& position() const
			{
				return pos_;
			}

			void set_position(const v3d& position)
			{
			    pos_ = position;
			}

			const v3d& velocity() const
			{
			    return vel_;
			}

			void set_velocity(const v3d& velocity)
			{
			    vel_ = velocity;
			}

			const colorf& color() const
			{
				return cur_color_;
			}

			void set_color(const colorf& c)
			{
			    cur_color_ = c;
			}

			bool alive() const
			{
				return is_alive_;
			}

			cmn::uint before_die() const
			{
			    return before_die_;
			}

			cmn::uint lifetime() const
			{
			    return lifetime_;
			}

			void set_lifetime(cmn::uint lftime)
			{
			    lifetime_ = lftime;
			}

			void set_size(float size)
			{
			    size_ = size;
			}

			const varray& get_vertexes() const
			{
			    return v;
			}

			const varray& get_normals() const
			{
			    return v;
			}

			const iarray& get_indices() const
			{
			    return i;
			}
	};
};

#endif
