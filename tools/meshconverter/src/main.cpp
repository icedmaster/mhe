#include <utils/logutils.hpp>
#include <utils/file_utils.hpp>
#include <utils/cmdline_parser.hpp>
#include <render/layouts.hpp>
#include <render/mesh_grid.hpp>
#include <render/mesh.hpp>
#include <res/mhe_binary_mesh.hpp>
#include <res/scene_export.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <pugixml/pugixml.hpp>
#include <vector>
#include <fstream>

const size_t mhe_header_length = 3;
const char mhe_header[mhe_header_length + 1] = "mhe";
const char mhe_default_version = 0x2;
const char mhe_animation_version = 0x1;

const size_t initial_vertex_buffer_size = 100000;
const size_t initial_animations_frames_number = 256;

const mhe::vec3 default_aabb_min = mhe::vec3(9999999.0, 9999999.0f, 9999999.0f);
const mhe::vec3 default_aabb_max = mhe::vec3(-9999999.0f, -9999999.0f, -9999999.0f);

struct ExportParams
{
    mhe::FilePath texture_path;
    mhe::FilePath material_path;
    uint8_t version;
};

struct ExportBone
{
    size_t index;
    float weight;
};

struct SkinningExportContext
{
    std::vector<mhe::Bone> bones;
    mhe::hashmap<mhe::string, size_t> bone_name_to_index;
    mhe::hashmap<uint32_t, std::vector<ExportBone> > vertices_to_bones;
    mhe::mat4x4 root_local_to_world_transform;
    mhe::mat4x4 inv_mesh_transform;
};

struct MeshExportContext
{
    mhe::res::Mesh mesh;
    mhe::vector<mhe::res::Material> materials;
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

mhe::mat4x4 convert(const aiMatrix4x4& m)
{
    mhe::mat4x4 r;
    r.set(m.a1, m.b1, m.c1, m.d1,
          m.a2, m.b2, m.c2, m.d2,
          m.a3, m.b3, m.c3, m.d3,
          m.a4, m.b4, m.c4, m.d4);
    return r;
}

mhe::quatf convert(const aiQuaternion& q)
{
    return mhe::quatf(q.x, q.y, q.z, q.w);
}

void write_header(std::ofstream& stream, uint8_t layout, uint8_t version)
{
    stream.write(mhe_header, mhe_header_length);
    stream.put(version);
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

template <class Str>
void write_string(std::ofstream& stream, const Str& str)
{
    size_t len = str.length();
    stream.write((const char*)&len, 4);
    stream.write(str.data(), len);
}

void write_parts_data(std::ofstream& stream, const mhe::MeshPartExportData* data, size_t size)
{
    stream.write((const char*)&size, 4);
    stream.write((const char*)data, size * sizeof(mhe::MeshPartExportData));
}

void write_grid_data(std::ofstream& stream, const mhe::MeshGrid& grid)
{

}

void write_skinning_data(std::ofstream& stream, const SkinningExportContext& skinning_context)
{
    if (skinning_context.bones.empty())
        return;
    uint32_t num = skinning_context.bones.size();
    const mhe::Bone* bones = &skinning_context.bones[0];
    stream.write((const char*)&skinning_context.root_local_to_world_transform, sizeof(mhe::mat4x4));
    stream.write((const char*)&num, sizeof(uint32_t));
    for (size_t i = 0; i < num; ++i)
    {
        write_string(stream, bones[i].name);
        stream.write((const char*)&bones[i].id, sizeof(uint32_t));
        stream.write((const char*)&bones[i].parent_id, sizeof(uint32_t));
        stream.write((const char*)&bones[i].inv_transform, sizeof(mhe::mat4x4));
        stream.write((const char*)&bones[i].local_transform, sizeof(mhe::mat4x4));
    }
}

aiNode* get_node(const aiScene* ai_scene, const char* node_name)
{
    if (ai_scene->mRootNode == nullptr) return nullptr;
    return ai_scene->mRootNode->FindNode(node_name);
}

void add_bone(const aiScene* ai_scene, const aiBone* ai_bone, SkinningExportContext& skinning_context, size_t buffer_offset)
{
    mhe::hashmap<mhe::string, size_t>::iterator it = skinning_context.bone_name_to_index.find(convert(ai_bone->mName));
    if (it != skinning_context.bone_name_to_index.end())
        return;
    mhe::Bone bone;
    bone.name = convert(ai_bone->mName);
    bone.id = skinning_context.bones.size();
    bone.inv_transform = convert(ai_bone->mOffsetMatrix);
    skinning_context.bones.push_back(bone);

    for (size_t j = 0; j < ai_bone->mNumWeights; ++j)
    {
        size_t vertex_id = buffer_offset + ai_bone->mWeights[j].mVertexId;

        ExportBone export_bone;
        export_bone.index = bone.id;
        export_bone.weight = ai_bone->mWeights[j].mWeight;
        skinning_context.vertices_to_bones[vertex_id].push_back(export_bone);
    }
}

mhe::mat4x4 calculate_parent_transform(const aiNode* node)
{
    mhe::mat4x4 m = convert(node->mTransformation);
    if (node->mParent)
        m *= calculate_parent_transform(node->mParent);
    return m;
}

void setup_parents(const aiScene* ai_scene, const aiMesh* mesh, SkinningExportContext& skinning_context)
{
    for (size_t i = 0, size = mesh->mNumBones; i < size; ++i)
    {
        aiBone* ai_bone = mesh->mBones[i];
        mhe::hashmap<mhe::string, size_t>::iterator it = skinning_context.bone_name_to_index.find(mhe::string(ai_bone->mName.C_Str()));
        ASSERT(it != skinning_context.bone_name_to_index.end(), "Can't find bone " << ai_bone->mName.C_Str());
        mhe::Bone& bone = skinning_context.bones[it->value];
        aiNode* node = get_node(ai_scene, ai_bone->mName.C_Str());
        ASSERT(node != nullptr, "Can't find node with name:" << ai_bone->mName.C_Str());
        bone.local_transform = convert(node->mTransformation);
        if (node->mParent != nullptr)
        {
            mhe::hashmap<mhe::string, size_t>::iterator it = skinning_context.bone_name_to_index.find(mhe::string(node->mParent->mName.C_Str()));
            if (it != skinning_context.bone_name_to_index.end())
            {
                bone.parent_id = it->value;
            }
            else
            {
                skinning_context.root_local_to_world_transform = calculate_parent_transform(node->mParent) * skinning_context.inv_mesh_transform;
            }
        }
    }
}

struct WeightSortHelper
{
    bool operator()(const ExportBone& b1, const ExportBone& b2) const
    {
        return b1.weight > b2.weight;
    }
};

void sort_by_weight(std::vector<ExportBone>& bones)
{
    std::sort(bones.begin(), bones.end(), WeightSortHelper());
}

void fill_skinned_vertices(mhe::vector<mhe::SkinnedGeometryLayout::Vertex> &out_vertices,
    mhe::vector<mhe::StandartGeometryLayout::Vertex>& vertices, SkinningExportContext& skinning_context)
{
    for (size_t i = 0, size = vertices.size(); i < size; ++i)
    {
        out_vertices[i].pos = vertices[i].pos;
        out_vertices[i].nrm = vertices[i].nrm;
        out_vertices[i].tng = vertices[i].tng;
        out_vertices[i].tex = vertices[i].tex;
    }

    for (mhe::hashmap<uint32_t, std::vector<ExportBone> >::iterator it = skinning_context.vertices_to_bones.begin(),
        end = skinning_context.vertices_to_bones.end(); it != end; ++it)
    {
        mhe::SkinnedGeometryLayout::Vertex& v = out_vertices[it->key];
        sort_by_weight(it->value);
        size_t n = mhe::min(it->value.size(), 4u);
        float weight_total = 0.0f;

        memset(v.ids, 0, sizeof(v.ids));
        memset(v.weights, 0, sizeof(v.weights));

        for (size_t i = 0; i < n; ++i)
        {
            v.ids[i] = it->value[i].index;
            v.weights[i] = it->value[i].weight;
            weight_total += v.weights[i];
        }

        float left = 1.0f - weight_total;
        float add = left / n;
        for (size_t i = 0; i < n; ++i)
            v.weights[i] += add;
    }
}

template <class V>
void process_node(const aiScene* assimp_scene, aiNode* node, const aiMatrix4x4& parent_transform, 
    mhe::vector<uint32_t>& indices, mhe::vector<V>& vertices,
    mhe::vector<mhe::MeshPartExportData>& parts,
    mhe::vec3& mesh_aabb_min, mhe::vec3& mesh_aabb_max,
    SkinningExportContext& skinning_context, MeshExportContext& mesh_context)
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

        mhe::res::Mesh::Part& res_part = mesh_context.mesh.parts.add();
        res_part.material = mesh_context.materials[mesh->mMaterialIndex].ref;

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
                indices.push_back(mesh->mFaces[i].mIndices[j] + part_data.vbuffer_offset);
            }
        }

        for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
        {
            mhe::StandartGeometryLayout::Vertex vertex;
            aiVector3D v = transform * mesh->mVertices[i];
            vertex.pos.set(v.x, v.y, v.z);
            aiVector3D n = normal_transform * mesh->mNormals[i];
            vertex.nrm.set(n.x, n.y, n.z);
            vertex.nrm.normalize();

            if (mesh->mTextureCoords[0] != nullptr)
            {
                aiVector3D t = mesh->mTextureCoords[0][i];
                vertex.tex.set(t.x, t.y);

                aiVector3D aiTng = normal_transform * mesh->mTangents[i];
                aiVector3D aiBitng = normal_transform * mesh->mBitangents[i];
                mhe::vec3 tng(aiTng.x, aiTng.y, aiTng.z);
                mhe::vec3 bitng(aiBitng.x, aiBitng.y, aiBitng.z);
                float sign = mhe::dot(bitng, mhe::cross(vertex.nrm, tng)) > 0.0f ? 1.0f : -1.0f;

                vertex.tng = mhe::vec4(tng.normalized(), sign);
            }

            vertices.push_back(vertex);

            mesh_aabb_max = mhe::max(mesh_aabb_max, vertex.pos);
            mesh_aabb_min = mhe::min(mesh_aabb_min, vertex.pos);
            submesh_aabb_max = mhe::max(submesh_aabb_max, vertex.pos);
            submesh_aabb_min = mhe::min(submesh_aabb_min, vertex.pos);
        }

        if (mesh->HasBones())
        {
            ASSERT(node->mNumMeshes < 2, "Currently only single mesh per export is supported");
            skinning_context.inv_mesh_transform = convert(transform.Inverse());
            size_t bones_number = mesh->mNumBones;
            for (size_t i = 0; i < bones_number; ++i)
            {
                aiBone* ai_bone = mesh->mBones[i];
                mhe::hashmap<mhe::string, size_t>::iterator it = skinning_context.bone_name_to_index.find(convert(ai_bone->mName));
                ASSERT(it == skinning_context.bone_name_to_index.end(), "Are bones duplicated?");
                mhe::Bone bone;
                bone.name = convert(ai_bone->mName);
                bone.id = skinning_context.bones.size();
                bone.parent_id = mhe::Bone::invalid_id;
                bone.inv_transform = skinning_context.inv_mesh_transform * convert(ai_bone->mOffsetMatrix);
                skinning_context.bones.push_back(bone);

                skinning_context.bone_name_to_index[convert(ai_bone->mName)] = bone.id;

                for (size_t j = 0; j < ai_bone->mNumWeights; ++j)
                {
                    size_t vertex_id = part_data.ibuffer_offset + ai_bone->mWeights[j].mVertexId;

                    ExportBone export_bone;
                    export_bone.index = bone.id;
                    export_bone.weight = ai_bone->mWeights[j].mWeight;
                    skinning_context.vertices_to_bones[vertex_id].push_back(export_bone);
                }
            }
        }

        setup_parents(assimp_scene, mesh, skinning_context);

        mesh_context.mesh.parts.back().aabb = mhe::AABBf::from_min_max(submesh_aabb_min, submesh_aabb_max);
    }

    for (unsigned int n = 0; n < node->mNumChildren; ++n)
        process_node(assimp_scene, node->mChildren[n], transform, indices, vertices, parts, mesh_aabb_min, mesh_aabb_max, skinning_context, mesh_context);
}

void create_grid(mhe::MeshGrid& grid, const std::vector<mhe::StandartGeometryLayout::Vertex>& vertices,
    const std::vector<uint32_t>& indices)
{
    /*mhe::MeshGridHelper helper(grid);
    ASSERT(indices.size() % 3 == 0, "Only triangulated meshes are supported");
    for (size_t i = 0, size = indices.size(); i < size; i += 3)
    {
        helper.add(vertices[indices[i]].pos, vertices[indices[i + 1]].pos, vertices[indices[i + 2]].pos);
    }*/
}

void write_animation(mhe::AnimationExportData& animation_data, const char* out_filename, const ExportParams& params)
{
    // write out data to the file with name "out_filename-<animation_name>.anim"
    mhe::FilePath filename = mhe::utils::get_file_name(mhe::string(out_filename));
    filename += "-";
    filename += animation_data.name;
    filename += ".anim";

    INFO_LOG("Exporting animation " << filename << " ...");

    std::ofstream stream(filename.c_str(), std::ios::binary);
    if (!stream.is_open())
    {
        ERROR_LOG("Can't open file " << filename << " for writing");
        return;
    }
    stream.write(mhe_header, mhe_header_length);
    stream.write(&mhe_animation_version, 1);
    stream.write(reinterpret_cast<const char*>(&animation_data.duration), sizeof(float));
    stream.write(reinterpret_cast<const char*>(&animation_data.fps), sizeof(float));
    stream.write(reinterpret_cast<const char*>(&animation_data.play_mode), sizeof(uint8_t));
    stream.write(reinterpret_cast<const char*>(&animation_data.frames_number), sizeof(uint32_t));
    for (size_t i = 0; i < animation_data.frames_number; ++i)
    {
        stream.write(reinterpret_cast<const char*>(&animation_data.frames[i].node_id), sizeof(uint32_t));
        stream.write(reinterpret_cast<const char*>(&animation_data.frames[i].position_frames_number), sizeof(uint32_t));
        stream.write(reinterpret_cast<const char*>(animation_data.frames[i].position_frames),
            animation_data.frames[i].position_frames_number * sizeof(mhe::PositionAnimationExportFrame));
        stream.write(reinterpret_cast<const char*>(&animation_data.frames[i].rotation_frames_number), sizeof(uint32_t));
        stream.write(reinterpret_cast<const char*>(animation_data.frames[i].rotation_frames),
            animation_data.frames[i].rotation_frames_number * sizeof(mhe::RotationAnimationExportFrame));
        stream.write(reinterpret_cast<const char*>(&animation_data.frames[i].scale_frames_number), sizeof(uint32_t));
        stream.write(reinterpret_cast<const char*>(animation_data.frames[i].scale_frames),
            animation_data.frames[i].scale_frames_number * sizeof(mhe::ScaleAnimationExportFrame));
    }

    stream.close();
    INFO_LOG("Export finished");
}

void process_animations(const aiScene* assimp_scene, const char* out_filename, const ExportParams& params, const SkinningExportContext& skinning_context)
{
    for (size_t i = 0; i < assimp_scene->mNumAnimations; ++i)
    {
        mhe::AnimationExportData animation_data;

        aiAnimation* ai_animation = assimp_scene->mAnimations[i];
        animation_data.duration = static_cast<float>(ai_animation->mDuration);
        animation_data.fps = static_cast<float>(ai_animation->mTicksPerSecond);
        animation_data.name = convert(ai_animation->mName);
        if (animation_data.name.empty())
            animation_data.name = "default";

        std::vector<mhe::NodeAnimationExportFrame> frames;
        typedef std::vector<mhe::PositionAnimationExportFrame> PositionFramesVector;
        typedef std::vector<mhe::RotationAnimationExportFrame> RotationFramesVector;
        typedef std::vector<mhe::ScaleAnimationExportFrame> ScaleFramesVector;

        std::vector<PositionFramesVector> positions;
        std::vector<RotationFramesVector> rotations;
        std::vector<ScaleFramesVector> scalings;

        for (unsigned int i = 0; i < ai_animation->mNumChannels; ++i)
        {
            aiNodeAnim* ai_node_anim = ai_animation->mChannels[i];
            const mhe::string& bone_name = convert(ai_node_anim->mNodeName);
            mhe::hashmap<mhe::string, size_t>::const_iterator it = skinning_context.bone_name_to_index.find(bone_name);
            if (it == skinning_context.bone_name_to_index.end()) continue;
            mhe::NodeAnimationExportFrame frame;
            frame.node_id = it->value;
            frame.position_frames_number = ai_node_anim->mNumPositionKeys;
            PositionFramesVector pos;
            for (size_t j = 0; j < ai_node_anim->mNumPositionKeys; ++j)
            {
                mhe::PositionAnimationExportFrame pos_frame;
                pos_frame.time = static_cast<float>(ai_node_anim->mPositionKeys[j].mTime);
                pos_frame.position = convert(ai_node_anim->mPositionKeys[j].mValue);
                pos.push_back(pos_frame);
            }
            positions.push_back(pos);
            frame.position_frames = &positions.back()[0];

            frame.rotation_frames_number = ai_node_anim->mNumRotationKeys;
            RotationFramesVector rot;
            for (size_t j = 0; j < ai_node_anim->mNumRotationKeys; ++j)
            {
                mhe::RotationAnimationExportFrame rot_frame;
                rot_frame.time = static_cast<float>(ai_node_anim->mRotationKeys[j].mTime);
                rot_frame.rotation = convert(ai_node_anim->mRotationKeys[j].mValue);
                rot.push_back(rot_frame);
            }
            rotations.push_back(rot);
            frame.rotation_frames = &rotations.back()[0];

            frame.scale_frames_number = ai_node_anim->mNumScalingKeys;
            ScaleFramesVector sc;
            for (size_t j = 0; j < ai_node_anim->mNumScalingKeys; ++j)
            {
                mhe::ScaleAnimationExportFrame scale_frame;
                scale_frame.time = static_cast<float>(ai_node_anim->mScalingKeys[j].mTime);
                scale_frame.scale = convert(ai_node_anim->mScalingKeys[j].mValue);
                sc.push_back(scale_frame);
            }
            scalings.push_back(sc);
            frame.scale_frames = &scalings.back()[0];

            frames.push_back(frame);
        }

        animation_data.frames_number = frames.size();
        animation_data.frames = frames.empty() ? nullptr : &frames[0];
        write_animation(animation_data, out_filename, params);
    }
}

void write_material_data(const mhe::FilePath& filename, const mhe::res::Material& material)
{
    mhe::res::XMLSerializer serializer(filename.c_str(), "material");
    material.write(serializer);
    INFO_LOG("Exported a material:" << filename);
}

void process_material_data(const char* out_filename, const ExportParams& params, MeshExportContext& mesh_context)
{
    for (size_t i = 0, size = mesh_context.materials.size(); i < size; ++i)
    {
        mhe::FilePath filename = mhe::utils::get_file_path(mhe::FilePath(out_filename)) + mesh_context.materials[i].id;
        filename += ".mtl";
        write_material_data(filename, mesh_context.materials[i]);
        mesh_context.materials[i].ref = params.material_path + mesh_context.materials[i].id;
    }
}

void write_mesh_data(const mhe::FilePath& binary_filename, const MeshExportContext& mesh_context)
{
    mhe::FilePath mesh_filename = mhe::utils::get_file_name_with_path(binary_filename) + ".mesh";
    mhe::res::XMLSerializer serializer(mesh_filename.c_str(), "mesh");
    mesh_context.mesh.write(serializer);
}

void process_scene(const char* out_filename, const aiScene* assimp_scene, const ExportParams& params)
{
    if (!assimp_scene->HasMeshes() && !assimp_scene->HasAnimations())
    {
        ERROR_LOG("This scene has neither meshes, nor animations");
        return;
    }
    
    mhe::vector<mhe::StandartGeometryLayout::Vertex> vertexes;
    mhe::vector<mhe::SkinnedGeometryLayout::Vertex> skinned_vertices;
    mhe::vector<uint32_t> indexes;
    mhe::vector<mhe::MeshPartExportData> parts(assimp_scene->mNumMeshes);

    SkinningExportContext skinning_context;
    MeshExportContext mesh_context;

    mesh_context.mesh.binary = mhe::utils::get_file_name_with_extension(out_filename);

    // init materials first
    mesh_context.materials.resize(assimp_scene->mNumMaterials);
    for (unsigned int i = 0; i < assimp_scene->mNumMaterials; ++i)
    {
        aiMaterial* material = assimp_scene->mMaterials[i];
        mhe::res::Material& material_data = mesh_context.materials[i];

        aiString name;
        material->Get(AI_MATKEY_NAME, name);
        material_data.id = name.C_Str();

        // textures
        aiString path;
        aiTextureMapMode map_mode;
        if (material->GetTexture(aiTextureType_DIFFUSE, 0, &path, 0, 0, 0, 0, &map_mode) == aiReturn_SUCCESS)
        {
            mhe::FilePath filename = path.C_Str();
            filename = mhe::utils::convert_slashes(filename);
            filename = filename.empty() ? filename : mhe::utils::path_join(params.texture_path, mhe::utils::get_file_name_with_extension(filename));
            material_data.albedo.ref = filename;
            INFO_LOG("Found an albedo texture:" << filename);
        }

        if (material->GetTexture(aiTextureType_HEIGHT, 0, &path, 0, 0, 0, 0, &map_mode) == aiReturn_SUCCESS)
        {
            mhe::FilePath filename = path.C_Str();
            filename = mhe::utils::convert_slashes(filename);
            filename = filename.empty() ? filename : mhe::utils::path_join(params.texture_path, mhe::utils::get_file_name_with_extension(filename));
            material_data.normalmap.ref = filename;
            INFO_LOG("Found a normalmap texture:" << filename);
        }
    }

    process_material_data(out_filename, params, mesh_context);

    vertexes.reserve(initial_vertex_buffer_size);
    indexes.reserve(initial_vertex_buffer_size);
    
    mhe::vec3 mesh_aabb_min = default_aabb_min, mesh_aabb_max = default_aabb_max;

    if (assimp_scene->mRootNode != nullptr)
    {
        parts.resize(0);
        process_node(assimp_scene, assimp_scene->mRootNode, aiMatrix4x4(), indexes, vertexes, parts, mesh_aabb_min, mesh_aabb_max, skinning_context, mesh_context);
    }

    mesh_context.mesh.aabb = mhe::AABBf::from_min_max(mesh_aabb_min, mesh_aabb_max);
    
    INFO_LOG("Exporting geometry " << out_filename << " ...");

    std::ofstream f(out_filename, std::ios::out | std::ios::binary);
    if (!f.is_open())
    {
        ERROR_LOG("Can't open file for writing:" << out_filename);
        return;
    }

    DEBUG_LOG("The result AABB is:" << mesh_context.mesh.aabb);

    uint8_t layout = mhe::StandartGeometryLayout::handle;
    size_t vertex_size = sizeof(mhe::StandartGeometryLayout::Vertex);
    const char* vertex_data = (const char*)&vertexes[0];

    if (!skinning_context.bones.empty())
    {
        INFO_LOG("The scene contains bones - SkinnedGeometryLayout will be exported");
        layout = mhe::SkinnedGeometryLayout::handle;
        vertex_size = sizeof(mhe::SkinnedGeometryLayout::Vertex);

        skinned_vertices.resize(vertexes.size());
        fill_skinned_vertices(skinned_vertices, vertexes, skinning_context);
        vertex_data = (const char*)&skinned_vertices[0];
    }

    // description data
    write_mesh_data(out_filename, mesh_context);

    // binary data
    write_header(f, layout, params.version);
    write_vertex_data(f, vertex_size,
        vertex_data, vertexes.size(),
        (const char*)&indexes[0], indexes.size());
    write_parts_data(f, &parts[0], parts.size());
    write_skinning_data(f, skinning_context);
    //write_grid_data(f, mesh_grid);

    f.close();

    INFO_LOG("Export finished");

    process_animations(assimp_scene, out_filename, params, skinning_context);
}

int main(int argc, char** argv)
{
    mhe::utils::create_standart_log();
    mhe::create_default_allocator();
    mhe::utils::CmdLineParser parser(argc, argv);
    uint8_t version = parser.get<uint8_t>("--version", "-v", mhe_default_version);
    const char* in_filename = parser.get<const char*>("--source", "-s");
    const char* out_filename = parser.get<const char*>("--destination", "-d");
    const char* texture_path = parser.get<const char*>("--texture-path", "-tp");
    const char* material_path = parser.get<const char*>("--material-path", "-mp");
    if (in_filename == nullptr || out_filename == nullptr)
    {
        ERROR_LOG("Source and destination names must be specified");
        return 1;
    }
    
    Assimp::Importer importer;
    const aiScene* assimp_scene = importer.ReadFile(in_filename, aiProcess_CalcTangentSpace | aiProcess_Triangulate |
        aiProcess_GenNormals | aiProcess_GenUVCoords | aiProcess_TransformUVCoords);
    if (!assimp_scene)
    {
        ERROR_LOG("Error occured during the scene parsing:" << importer.GetErrorString());
        return 2;
    }

    ExportParams params;
    params.version = version;
    if (texture_path != nullptr)
        params.texture_path = texture_path;
    if (material_path != nullptr)
        params.material_path = material_path;

    process_scene(out_filename, assimp_scene, params);

    mhe::destroy_default_allocator();

    return 0;
}
