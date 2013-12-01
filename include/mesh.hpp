#ifndef __MESH_HPP__
#define __MESH_HPP__

#include <boost/shared_ptr.hpp>
#include "fixed_size_vector.hpp"
#include "engine_config.hpp"
#include "transform.hpp"

namespace mhe {

struct Mesh
{
	typedef fixed_size_vector<float, initial_number_of_vertexes> vertexes_vector;
	typedef fixed_size_vector<float, initial_number_of_texcoords> texcoord_vector;
	typedef fixed_size_vector<unsigned int, initial_number_of_vertexes> indexes_vector;

	Mesh() :
		flags(0)
	{}

	Mesh(const Mesh& other);

	void apply_transform(const Transform& transform);
	void clear();
	
	vertexes_vector vertexes;
	vertexes_vector normals;
	texcoord_vector uv;
	indexes_vector indexes;
	uint32_t flags;
};

typedef boost::shared_ptr<Mesh> mesh_ptr;

void batch(Mesh& to, const Mesh& from, const Transform& from_transform);
Mesh* create_mesh(const std::pair<const Mesh&, const Transform&>* meshes, size_t size);
inline Mesh* create_mesh(const std::pair<const Mesh&, const Transform&>& mesh1,
						 const std::pair<const Mesh&, const Transform&>& mesh2)
{
	std::pair<const Mesh&, const Transform&> meshes[2] =
	{
	    mesh1, mesh2
	};
	return create_mesh(meshes, 2);
}

}

#endif
