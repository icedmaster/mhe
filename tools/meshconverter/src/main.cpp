#include <utils/logutils.hpp>
#include <utils/file_utils.hpp>
#include <render/layouts.hpp>
#include <render/mesh_grid.hpp>
#include <res/scene_export.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>
#include <fstream>

const size_t mhe_header_length = 3;
const char mhe_header[mhe_header_length + 1] = "mhe";
const char mhe_version = 0x2;

const size_t initial_vertex_buffer_size = 100000;

const mhe::vec3 default_aabb_min = mhe::vec3(9999999.0, 9999999.0f, 9999999.0f);
const mhe::vec3 default_aabb_max = mhe::vec3(-9999999.0f, -9999999.0f, -9999999.0f);

struct ExportParams
{
	mhe::FilePath texture_path;
};

mhe::vec3 convert(const aiVector3D& v)
{
	return mhe::vec3(v.x, v.y, v.z);
}

mhe::vec3 convert(const aiColor3D& v)
{
	return mhe::vec3(v.r, v.g, v.b);
}

mhe::string convert(const aiString& s)
{
	return mhe::string(s.C_Str());
}

void write_header(std::ofstream& stream, uint8_t layout)
{
	stream.write(mhe_header, mhe_header_length);
	stream.put(mhe_version);
	stream.put(layout);
}

void write_vertex_data(std::ofstream& stream, uint32_t vertex_size, const char* vertex_data, uint32_t vertexes_number,
	const char* index_data, uint32_t indexes_number)
{
	stream.write((const char*)&vertex_size, 4);
	stream.write((const char*)&vertexes_number, 4);
	stream.write(vertex_data, vertex_size * vertexes_number);
	stream.write((const char*)&indexes_number, 4);
	stream.write(index_data, indexes_number * 4); // index is u32
}

void write_mesh_data(std::ofstream& stream, const mhe::MeshExportData& data)
{
	stream.write((const char*)&data, sizeof(mhe::MeshExportData));
}

template <class Str>
void write_string(std::ofstream& stream, const Str& str)
{
	size_t len = str.length();
	stream.write((const char*)&len, 4);
	stream.write(str.data(), len);
}

void write_material_data(std::ofstream& stream, const mhe::MaterialExportData* data, size_t size)
{
	uint32_t materials_number = size;
	stream.write((const char*)&materials_number, 4);
	for (size_t i = 0; i < size; ++i)
	{
		// name
		write_string(stream, data[i].name);
		// material_system
		write_string(stream, data[i].material_system);
		// lighting model
		write_string(stream, data[i].lighting_model);
		// material data
		stream.write((const char*)&data[i].data, sizeof(mhe::MaterialRenderData));
		// textures
		uint32_t len = data[i].albedo_texture.name.length();
		stream.write((const char*)&len, 4);
		stream.write(data[i].albedo_texture.name.c_str(), len);
		stream.write((const char*)&(data[i].albedo_texture.mode), 1);

		len = data[i].normalmap_texture.name.length();
		stream.write((const char*)&len, 4);
		stream.write(data[i].normalmap_texture.name.c_str(), len);
		stream.write((const char*)&(data[i].normalmap_texture.mode), 1);
	}
}

void write_parts_data(std::ofstream& stream, const mhe::MeshPartExportData* data, size_t size)
{
	stream.write((const char*)&size, 4);
	stream.write((const char*)data, size * sizeof(mhe::MeshPartExportData));
}

void write_grid_data(std::ofstream& stream, const mhe::MeshGrid& grid)
{

}

void process_node(const aiScene* assimp_scene, aiNode* node, const aiMatrix4x4& parent_transform, 
	std::vector<uint32_t>& indices, std::vector<mhe::StandartGeometryLayout::Vertex>& vertices,
	std::vector<mhe::MeshPartExportData>& parts, mhe::vec3& mesh_aabb_min, mhe::vec3& mesh_aabb_max)
{
	aiMatrix4x4 transform = parent_transform * node->mTransformation;
	aiMatrix4x4 inv_transform = transform;
	inv_transform.Inverse().Transpose();
	aiMatrix3x3 normal_transform(inv_transform);

	mhe::mat4x4 mhe_normal_transform;
	mhe_normal_transform.set(normal_transform.a1, normal_transform.b1, normal_transform.c1, 0.0f,
							 normal_transform.a2, normal_transform.b2, normal_transform.c2, 0.0f,
							 normal_transform.a3, normal_transform.b3, normal_transform.c3, 0.0f,
							 0.0f, 0.0f, 0.0f, 0.0f);
	for (unsigned int m = 0; m < node->mNumMeshes; ++m)
	{
		aiMesh* mesh = assimp_scene->mMeshes[node->mMeshes[m]];
		mhe::MeshPartExportData part_data;
		part_data.ibuffer_offset = indices.size();
		part_data.vbuffer_offset = vertices.size();
		part_data.faces_number = mesh->mNumFaces;
		part_data.material_index = mesh->mMaterialIndex;

		mhe::vec3 submesh_aabb_min = default_aabb_min, submesh_aabb_max = default_aabb_max;

		parts.push_back(part_data);

		if (mesh->mTextureCoords[0] == nullptr)
		{
			WARN_LOG("The mesh with index:" << m << " doesn't contain any UV");
		}

		for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
		{
			ASSERT(mesh->mFaces[i].mNumIndices == 3, "The mesh must be triangulated");
			for (unsigned int j = 0; j < mesh->mFaces[i].mNumIndices; ++j)
			{
				indices.push_back(mesh->mFaces[i].mIndices[j]);
			}
		}

		for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
		{
			mhe::StandartGeometryLayout::Vertex vertex;
			aiVector3D v = transform * mesh->mVertices[i];
			vertex.pos.set(v.x, v.y, v.z);
			aiVector3D n = normal_transform * mesh->mNormals[i];
			vertex.nrm.set(n.x, n.y, n.z);

			if (mesh->mTextureCoords[0] != nullptr)
			{
				aiVector3D t = mesh->mTextureCoords[0][i];
				vertex.tex.set(t.x, t.y);

				aiVector3D aiTng = normal_transform * mesh->mTangents[i];
				aiVector3D aiBitng = normal_transform * mesh->mBitangents[i];
				mhe::vec3 tng(aiTng.x, aiTng.y, aiTng.z);
				mhe::vec3 bitng(aiBitng.x, aiBitng.y, aiBitng.z);
				float sign = mhe::dot(bitng, mhe::cross(vertex.nrm, tng)) > 0.0f ? 1.0f : -1.0f;

				vertex.tng = mhe::vec4(tng, sign);
			}

			vertices.push_back(vertex);

			mesh_aabb_max = mhe::max(mesh_aabb_max, vertex.pos);
			mesh_aabb_min = mhe::min(mesh_aabb_min, vertex.pos);
			submesh_aabb_max = mhe::max(submesh_aabb_max, vertex.pos);
			submesh_aabb_min = mhe::min(submesh_aabb_min, vertex.pos);
		}
		parts.back().aabb = mhe::AABBf::from_min_max(submesh_aabb_min, submesh_aabb_max);
	}

	for (unsigned int n = 0; n < node->mNumChildren; ++n)
		process_node(assimp_scene, node->mChildren[n], transform, indices, vertices, parts, mesh_aabb_min, mesh_aabb_max);
}

void create_grid(mhe::MeshGrid& grid, const std::vector<mhe::StandartGeometryLayout::Vertex>& vertices,
	const std::vector<uint32_t>& indices)
{
	mhe::MeshGridHelper helper(grid);
	ASSERT(indices.size() % 3 == 0, "Only triangulated meshes are supported");
	for (size_t i = 0, size = indices.size(); i < size; i += 3)
	{
		helper.add(vertices[indices[i]].pos, vertices[indices[i + 1]].pos, vertices[indices[i + 2]].pos);
	}
}

void process_scene(const char* out_filename, const aiScene* assimp_scene, const ExportParams& params)
{
    if (!assimp_scene->HasMeshes())
    {
        ERROR_LOG("This scene has no meshes");
        return;
    }
	
	mhe::MeshExportData mesh_export_data;
	std::vector<mhe::StandartGeometryLayout::Vertex> vertexes;
	std::vector<uint32_t> indexes;
	std::vector<mhe::MeshPartExportData> parts(assimp_scene->mNumMeshes);
	std::vector<mhe::MaterialExportData> materials(assimp_scene->mNumMaterials);

	vertexes.reserve(initial_vertex_buffer_size);
	indexes.reserve(initial_vertex_buffer_size);
	
	mhe::vec3 mesh_aabb_min = default_aabb_min, mesh_aabb_max = default_aabb_max;

	if (assimp_scene->mRootNode != nullptr)
	{
		parts.resize(0);
		process_node(assimp_scene, assimp_scene->mRootNode, aiMatrix4x4(), indexes, vertexes, parts, mesh_aabb_min, mesh_aabb_max);
	}
	else
	{
		for (unsigned int m = 0; m < assimp_scene->mNumMeshes; ++m)
		{
			aiMesh* mesh = assimp_scene->mMeshes[m];
			ASSERT(mesh != nullptr, "Mesh is invalid");

			mhe::MeshPartExportData part_data;
			part_data.ibuffer_offset = indexes.size();
			part_data.vbuffer_offset = vertexes.size();
			part_data.faces_number = mesh->mNumFaces;
			part_data.material_index = mesh->mMaterialIndex;

			mhe::vec3 submesh_aabb_min = default_aabb_min, submesh_aabb_max = default_aabb_max;

			parts[m] = part_data;

			for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
			{
				ASSERT(mesh->mFaces[i].mNumIndices == 3, "The mesh must be triangulated");
				for (unsigned int j = 0; j < mesh->mFaces[i].mNumIndices; ++j)
				{
					indexes.push_back(mesh->mFaces[i].mIndices[j]);
				}
			}

			for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
			{
				mhe::StandartGeometryLayout::Vertex vertex;
				aiVector3D& v = mesh->mVertices[i];
				vertex.pos.set(v.x, v.y, v.z);
				aiVector3D& n = mesh->mNormals[i];
				vertex.nrm.set(n.x, n.y, n.z);
				aiVector3D& t = mesh->mTextureCoords[0][i];
				vertex.tex.set(t.x, t.y);

				aiVector3D& aiTng = mesh->mTangents[i];
				aiVector3D& aiBitng = mesh->mBitangents[i];
				mhe::vec3 tng(aiTng.x, aiTng.y, aiTng.z);
				mhe::vec3 bitng(aiBitng.x, aiBitng.y, aiBitng.z);
				float sign = mhe::dot(bitng, mhe::cross(vertex.nrm, tng)) > 0.0f ? 1.0f : -1.0f;

				vertex.tng = mhe::vec4(tng, sign);

				vertexes.push_back(vertex);

				mesh_aabb_max = mhe::max(mesh_aabb_max, vertex.pos);
				mesh_aabb_min = mhe::min(mesh_aabb_min, vertex.pos);
				submesh_aabb_max = mhe::max(submesh_aabb_max, vertex.pos);
				submesh_aabb_min = mhe::min(submesh_aabb_min, vertex.pos);
			}
			parts[m].aabb = mhe::AABBf::from_min_max(submesh_aabb_min, submesh_aabb_max);
		}
	}

	mesh_export_data.aabb = mhe::AABBf::from_min_max(mesh_aabb_min, mesh_aabb_max);

	for (unsigned int i = 0; i < assimp_scene->mNumMaterials; ++i)
	{
		aiMaterial* material = assimp_scene->mMaterials[i];
		mhe::MaterialExportData& material_data = materials[i];

		aiString name;
		material->Get(AI_MATKEY_NAME, name);

		// render properties
		aiColor3D diffuse, ambient, specular, emissive;
		float shininess = 0.0f, shininess_strength = 0.0f;
		material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);
		material->Get(AI_MATKEY_COLOR_AMBIENT, ambient);
		material->Get(AI_MATKEY_COLOR_SPECULAR, specular);
		material->Get(AI_MATKEY_COLOR_EMISSIVE, emissive);
		material->Get(AI_MATKEY_SHININESS, shininess);
		material->Get(AI_MATKEY_SHININESS_STRENGTH, shininess_strength);

		material_data.name = convert(name);
		material_data.data.diffuse = convert(diffuse);
		material_data.data.ambient = convert(ambient);
		material_data.data.specular = convert(specular);
		material_data.data.emissive = convert(emissive);
		material_data.data.specular_shininess = shininess;

		// textures
		aiString path;
		aiTextureMapMode map_mode;
		if (material->GetTexture(aiTextureType_DIFFUSE, 0, &path, 0, 0, 0, 0, &map_mode) == aiReturn_SUCCESS)
		{
			mhe::FilePath filename = path.C_Str();
			filename = mhe::utils::convert_slashes(filename);
			filename = filename.empty() ? filename : mhe::utils::path_join(params.texture_path, mhe::utils::get_file_name_with_extension(filename));
			material_data.albedo_texture.name = filename;
			material_data.albedo_texture.mode = static_cast<uint8_t>(map_mode);
		}

		if (material->GetTexture(aiTextureType_HEIGHT, 0, &path, 0, 0, 0, 0, &map_mode) == aiReturn_SUCCESS)
		{
			mhe::FilePath filename = path.C_Str();
			filename = mhe::utils::convert_slashes(filename);
			filename = filename.empty() ? filename : mhe::utils::path_join(params.texture_path, mhe::utils::get_file_name_with_extension(filename));
			material_data.normalmap_texture.name = filename;
			material_data.normalmap_texture.mode = static_cast<uint8_t>(map_mode);
		}
	}

	//mhe::MeshGrid mesh_grid;
	//create_grid(mesh_grid, vertexes, indexes);

	std::ofstream f(out_filename, std::ios::out | std::ios::binary);
	if (!f.is_open())
	{
		ERROR_LOG("Can't open file for writing:" << out_filename);
		return;
	}

	DEBUG_LOG("The result AABB is:" << mesh_export_data.aabb);

	write_header(f, 0);
	write_mesh_data(f, mesh_export_data);
	write_vertex_data(f, sizeof(mhe::StandartGeometryLayout::Vertex),
		(const char*)&vertexes[0], vertexes.size(),
		(const char*)&indexes[0], indexes.size());
	write_material_data(f, &materials[0], materials.size());
	write_parts_data(f, &parts[0], parts.size());
	//write_grid_data(f, mesh_grid);

	f.close();
}

int main(int argc, char** argv)
{
	mhe::utils::create_standart_log();
	if (argc < 3)
	{
		ERROR_LOG("Invalid number of arguments");
		ERROR_LOG("Usage: meshconverter infile outfile");
		return 1;
	}

	const char* in_filename = argv[1];
	const char* out_filename = argv[2];
	const char* texture_path = argc > 3 ? argv[3] : nullptr;
	
	Assimp::Importer importer;
	const aiScene* assimp_scene = importer.ReadFile(in_filename, aiProcess_CalcTangentSpace | aiProcess_Triangulate |
        aiProcess_GenNormals | aiProcess_GenUVCoords | aiProcess_TransformUVCoords);
	if (!assimp_scene)
	{
		ERROR_LOG("Error occured during the scene parsing:" << importer.GetErrorString());
		return 2;
	}

	ExportParams params;
	if (texture_path != nullptr)
		params.texture_path = texture_path;

	process_scene(out_filename, assimp_scene, params);

	return 0;
}
