#ifndef _IMESH_HPP_
#define _IMESH_HPP_

#include "material.hpp"
#include "texture.hpp"
#include <vector>
#include <boost/shared_ptr.hpp>

namespace mhe
{
	class iMesh
	{
		private:
			virtual void set_vertexes(const std::vector<float>&) = 0;
			virtual void set_indexes(const std::vector<cmn::uint>&) = 0;
			virtual const std::vector<float>& get_vertexes() const = 0;
			virtual void set_normals(const std::vector<float>&) = 0;
			virtual void set_texture(const boost::shared_ptr<iTexture>&) = 0;
			virtual void add_submesh(const boost::shared_ptr<iMesh>&) = 0;
			virtual void draw_impl() = 0;
		public:
			void setVertexes(const std::vector<float>& v, const std::vector<cmn::uint>& i)
			{
				set_vertexes(v);
				set_indexes(i);
			}
			
			const std::vector<float>& getVertexes() const
			{
				return get_vertexes();
			}
			
			void setNormals(const std::vector<float>& n)
			{
				set_normals(n);
			}
			
			void setTexture(const boost::shared_ptr<iTexture>& t)
			{
				set_texture(t);
			}
			
			void addSubmesh(const boost::shared_ptr<iMesh>& sm)
			{
				add_submesh(sm);
			}
			
			void draw()
			{
				draw_impl();
			}						
	};
	
};

#endif
