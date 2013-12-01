#include "mesh.hpp"

namespace mhe {

Mesh::Mesh(const Mesh& other) :
	vertexes(other.vertexes),
	normals(other.normals),
	uv(other.uv),
	indexes(other.indexes),
	flags(other.flags)
{}

void Mesh::apply_transform(const Transform& transform)
{
	for (size_t i = 0; i < vertexes.size(); i += 3)
	{
		vector3<float> v(vertexes[i], vertexes[i + 1], vertexes[i + 2]);
		v = v * transform.transform();
		vertexes[i] = v.x(); vertexes[i + 1] = v.y(); vertexes[i + 2] = v.z();
	}	
}

void Mesh::clear()
{
	vertexes.clear();
	normals.clear();
	uv.clear();
	indexes.clear();
	flags = 0;
}

void batch(Mesh& to, const Mesh& from, const Transform& from_transform)
{
	size_t v_sz = to.vertexes.size() / 3;
	for (size_t i = 0; i < from.vertexes.size(); i += 3)
	{
		vector3<float> v(from.vertexes[i], from.vertexes[i + 1], from.vertexes[i + 2]);
		v = v * from_transform.transform();
		to.vertexes.insert(to.vertexes.end(), v.get(), v.get() + 3);
	}
	to.normals.insert(to.normals.end(), from.normals.begin(), from.normals.end());	
	to.uv.insert(to.uv.end(), from.uv.begin(), from.uv.end());
	for (size_t i = 0; i < from.indexes.size(); ++i)
		to.indexes.push_back(from.indexes[i] + v_sz); 
	to.flags |= from.flags;
}

Mesh* create_mesh(const std::pair<const Mesh&, const Transform&>* meshes, size_t size)
{
	assert(meshes != nullptr);
	if (!size) return nullptr;
	Mesh* mesh = new Mesh(meshes[0].first);
	mesh->apply_transform(meshes[0].second);
	for (size_t i = 1; i < size; ++i)
		batch(*mesh, meshes[i].first, meshes[i].second);
	return mesh;
}

}
