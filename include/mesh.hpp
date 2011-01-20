#ifndef _MESH_HPP_
#define _MESH_HPP_

#include "imesh.hpp"
#include "mhe_gl.hpp"

namespace mhe
{
	class Mesh : public iMesh
	{
		private:
			std::vector<float> v_;	// vertexes
			std::vector<cmn::uint> i_;	// vertex indexes
			std::vector<float> n_;	// normals
			boost::shared_ptr<iTexture> t_;	// texture
			std::vector< boost::shared_ptr<iMesh> > sm_;	// submeshes 
			cmn::uint numfaces;
		private:
			void set_vertexes(const std::vector<float>& v)
			{
				v_ = v;
			}
			
			void set_indexes(const std::vector<cmn::uint>& i)
			{
				i_ = i;
				numfaces = i.size() / 3;	// draw as triangles
			}
			
			const std::vector<float>& get_vertexes() const
			{
				return v_;
			}
			
			void set_normals(const std::vector<float>& n)
			{
				n_ = n;
			}
			
			void set_texture(const boost::shared_ptr<iTexture>& t)
			{
				t_ = t;
			}
			
			void add_submesh(const boost::shared_ptr<iMesh>& sm)
			{
				sm_.push_back(sm);
			}
			
			void draw_impl();
	};
	
};

#endif
