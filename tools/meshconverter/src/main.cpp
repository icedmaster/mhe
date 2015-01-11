#include <utils/logutils.hpp>
#include <render/layouts.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>
#include <fstream>

const size_t mhe_header_length = 3;
const char mhe_header[mhe_header_length + 1] = "mhe";
const char mhe_version = 0x1;

void write_output(const char* filename, uint8_t layout, uint32_t vertex_size, const char* vertex_data, uint32_t vertex_data_size,
	const char* index_data, uint32_t index_data_size)
{
	std::ofstream f(filename);
	if (!f.is_open())
	{
		ERROR_LOG("Can't open file for writing:" << filename);
		return;
	}
	f.write(mhe_header, mhe_header_length);
	f.put(mhe_version);
	f.put(layout);
	f.write((const char*)&vertex_size, 4);
	f.write((const char*)&vertex_data_size, 4);
	f.write(vertex_data, vertex_data_size);
	f.write((const char*)&index_data_size, 4);
	f.write(index_data, index_data_size);

	f.close();
}

void process_scene(const char* out_filename, const aiScene* assimp_scene)
{
	// get only first mesh for now
    if (!assimp_scene->HasMeshes())
    {
        ERROR_LOG("This scene has no meshes");
        return;
    }
	aiMesh* mesh = assimp_scene->mMeshes[0];
	
	std::vector<mhe::StandartGeometryLayout::Vertex> vertexes;
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

	std::vector<uint32_t> indexes;
	for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
	{
		ASSERT(mesh->mFaces[i].mNumIndices == 3, "The mesh must be triangulated");
		for (unsigned int j = 0; j < mesh->mFaces[i].mNumIndices; ++j)
			indexes.push_back(mesh->mFaces[i].mIndices[j]);
	}

	write_output(out_filename, 0, sizeof(mhe::StandartGeometryLayout::Vertex),
		(const char*)&vertexes[0], vertexes.size() * sizeof(mhe::StandartGeometryLayout::Vertex),
		(const char*)&indexes[0], indexes.size() * 4);
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
	
	Assimp::Importer importer;
	const aiScene* assimp_scene = importer.ReadFile(in_filename, aiProcess_CalcTangentSpace |
        aiProcess_Triangulate | aiProcess_GenNormals);
	if (!assimp_scene)
	{
		ERROR_LOG("Error occured during the scene parsing:" << importer.GetErrorString());
		return 2;
	}

	process_scene(out_filename, assimp_scene);

	return 0;
}
