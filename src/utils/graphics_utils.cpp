#include "utils/graphics_utils.hpp"

#include "sprite.hpp"

namespace mhe {
namespace utils {

namespace {

const std::string sprite_shader = "diffuse_unlit";

}

Sprite* create_sprite(const std::string& filename, const Context& context)
{
	boost::shared_ptr<Texture> texture =
		context.texture_manager().get(filename);
	Sprite* sprite = new Sprite();
	sprite->set_material(material_ptr(new Material(texture, context.shader_manager().get(sprite_shader))));
	return sprite;
}

Sprite* create_sprite(const colorf& color, const mhe::vector2<float>& size, const Context& context)
{
	Sprite* sprite = new Sprite;
	sprite->set_color(color);
	sprite->set_size(size.x(), size.y());
	sprite->set_material(material_ptr(new Material(context.texture_manager().get(default_resource_name),
												   context.shader_manager().get(sprite_shader))));
	return sprite;
}

mesh_ptr create_sprite_mesh()
{
	static mesh_ptr mesh;
	if (mesh != nullptr)
		return mesh;
	mesh.reset(new Mesh);
	mesh->vertexes.resize(12);
	Mesh::vertexes_vector& v = mesh->vertexes;
	v[0] = 0; v[1] = 0; v[2] = 0;
	v[3] = 0; v[4] = 1; v[5] = 0;
	v[6] = 1; v[7] = 1; v[8] = 0;
	v[9] = 1; v[10] = 0; v[11] = 0;
	mesh->normals.resize(12);
	mesh->uv.resize(8);
	Mesh::texcoord_vector& uv = mesh->uv;
	uv[0] = 0; uv[1] = 0; uv[2] = 0; uv[3] = 1;
	uv[4] = 1; uv[5] = 1; uv[6] = 1; uv[7] = 0;
	mesh->indexes.resize(6);
	Mesh::indexes_vector& i = mesh->indexes;
	i[0] = 0; i[1] = 1; i[2] = 2;
	i[3] = 2; i[4] = 3; i[5] = 0;

	return mesh;
}

}}
