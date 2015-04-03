#include <utils/logutils.hpp>
#include <utils/file_utils.hpp>
#include <render/layouts.hpp>
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

struct ExportParams
{
	mhe::FilePath texture_path;
};

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

void write_material_data(std::ofstream& stream, const mhe::MaterialExportData* data, size_t size)
{
	uint32_t materials_number = size;
	stream.write((const char*)&materials_number, 4);
	for (size_t i = 0; i < size; ++i)
	{
		stream.put('a'); // albedo
		uint32_t len = data[i].albedo_texture.name.length();
		stream.write((const char*)&len, 4);
		stream.write(data[i].albedo_texture.name.c_str(), len);
		stream.write((const char*)&(data[i].albedo_texture.mode), 1);

		stream.put('n');
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

void process_scene(const char* out_filename, const aiScene* assimp_scene, const ExportParams& params)
{
	// get only first mesh for now
    if (!assimp_scene->HasMeshes())
    {
        ERROR_LOG("This scene has no meshes");
        return;
    }
	
	std::vector<mhe::StandartGeometryLayout::Vertex> vertexes;
	std::vector<uint32_t> indexes;
	std::vector<mhe::MeshPartExportData> parts(assimp_scene->mNumMeshes);
	std::vector<mhe::MaterialExportData> materials(assimp_scene->mNumMaterials);

	vertexes.reserve(initial_vertex_buffer_size);
	indexes.reserve(initial_vertex_buffer_size);

	for (unsigned int m = 0; m < assimp_scene->mNumMeshes; ++m)
	{
		aiMesh* mesh = assimp_scene->mMeshes[m];
		ASSERT(mesh != nullptr, "Mesh is invalid");

		mhe::MeshPartExportData part_data;
		part_data.ibuffer_offset = indexes.size();
		part_data.vbuffer_offset = vertexes.size();
		part_data.faces_number = mesh->mNumFaces;
		part_data.material_index = mesh->mMaterialIndex;

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
			aiVector3D& tng = mesh->mTangents[i];
			vertex.tng.set(tng.x, tng.y, tng.z);

			vertexes.push_back(vertex);
		}
	}

	for (unsigned int i = 0; i < assimp_scene->mNumMaterials; ++i)
	{
		aiMaterial* material = assimp_scene->mMaterials[i];
		mhe::MaterialExportData& material_data = materials[i];

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

	std::ofstream f(out_filename, std::ios::out | std::ios::binary);
	if (!f.is_open())
	{
		ERROR_LOG("Can't open file for writing:" << out_filename);
		return;
	}

	write_header(f, 0);
	write_vertex_data(f, sizeof(mhe::StandartGeometryLayout::Vertex),
		(const char*)&vertexes[0], vertexes.size(),
		(const char*)&indexes[0], indexes.size());
	write_material_data(f, &materials[0], materials.size());
	write_parts_data(f, &parts[0], parts.size());

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
        aiProcess_GenNormals | aiProcess_GenUVCoords | aiProcess_RemoveRedundantMaterials | aiProcess_TransformUVCoords);
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