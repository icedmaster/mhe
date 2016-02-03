#ifndef __MATERIAL_SYSTEM_HPP__
#define __MATERIAL_SYSTEM_HPP__

#include "core/ref_counter.hpp"
#include "core/hash.hpp"
#include "core/factory.hpp"
#include "core/keyvalue.hpp"

#include "shader_program.hpp"
#include "render_target.hpp"

namespace mhe {

struct Context;
struct Material;
struct MaterialInstance;
struct RenderContext;
struct ModelContext;
struct SceneContext;
struct Mesh;
struct TextureInstance;
struct NodeInstance;
struct TransformInstance;
struct RenderPass;
struct MaterialNodes;
struct MeshPart;
struct DrawCall;
struct MeshPartInstance;
struct MeshInstance;

class Transform;
class RenderCommand;

static const size_t max_material_definitions = 8;

struct MaterialSystemContext
{
    string instance_name;
    string shader_name;
    string defs[max_material_definitions];
    KeyValue<string, string> options;
    size_t material_instances_number;
    size_t priority;
};

class MHE_EXPORT MaterialSystem : public ref_counter
{
public:
    virtual ~MaterialSystem() {}
    
    virtual bool init(Context& context, const MaterialSystemContext& material_system_context) = 0;
    virtual void close() = 0;

    virtual void setup(Context &context, SceneContext &scene_context, MeshPartInstance* instance_parts, MeshPart* parts, ModelContext* model_contexts, size_t count) = 0;

    virtual void set_texture(const TextureInstance& /*texture*/) {}
    virtual void set_texture(size_t /*unit*/, const TextureInstance& /*texture*/) {}

    virtual void start_frame(Context&, SceneContext&, RenderContext&) {}

    virtual void output(Context&, size_t /*unit*/, TextureInstance& /*texture*/) const {}

    virtual RenderTarget::IdType render_target_id() const
    {
        return default_render_target;
    }

    virtual size_t default_instances_number() const
    {
        return 0;
    }

    virtual void init_debug_views(Context& /*context*/)
    {}

    void setup_draw_calls(Context& context, SceneContext& scene_context, RenderContext& render_context);

    uint8_t id() const
    {
        return id_;
    }

    void set_id(uint8_t id)
    {
        id_ = id;
    }

    uint8_t priority() const
    {
        return priority_;
    }

    void set_priority(uint8_t priority)
    {
        priority_ = priority;
    }

    const char* name() const
    {
        return name_impl();
    }

    void enable()
    {
        enabled_ = true;
    }

    void disable()
    {
        enabled_ = false;
    }

    bool enabled() const
    {
        return enabled_;
    }

    void set_instance_name(const string& name)
    {
        instance_name_ = name;
    }

    const string& instance_name() const
    {
        return instance_name_;
    }
protected:
    bool init_default(Context& context, const MaterialSystemContext& material_system_context);

    void standart_material_setup(Context& context, SceneContext& scene_context, MeshPartInstance* instance_parts, MeshPart* part, ModelContext* model_contexts, size_t count);
    Transform& transform(const NodeInstance& node, const SceneContext& scene_context) const;

    void empty_setup(Context& context, SceneContext& scene_context, MeshPartInstance* instance_parts, MeshPart* parts, ModelContext* model_contexts, size_t count);
    void empty_update(Context& context, SceneContext& scene_context, RenderContext& render_context, MeshPartInstance* parts, size_t count);

    void set_layout(size_t layout)
    {
        layout_ = layout;
    }

    size_t layout() const
    {
        return layout_;
    }

    void set_shader(Shader shader)
    {
        shader_ = shader;
    }

    Shader& shader()
    {
        return shader_;
    }

    UberShader& ubershader(const Context& context) const;
    ShaderProgram& default_program(const Context& context) const;

    void setup_draw_call(DrawCall& draw_call, const MeshPartInstance& instance_part, const MeshPart& part, RenderCommand* command = nullptr) const;
private:
    virtual const char* name_impl() const = 0;
    virtual void update(Context& context, SceneContext& scene_context, RenderContext& render_context) = 0;
    virtual void setup_uniforms(Material& /*material*/, Context& /*context*/, SceneContext& /*scene_context*/, const MeshPartInstance& /*parts*/, const ModelContext& /*model_context*/) {}

    string instance_name_;
    Shader shader_;
    size_t layout_;
    uint8_t id_;
    uint8_t priority_;
    bool enabled_;
};

typedef Factory<MaterialSystem> MaterialSystemFactory;

#define SETUP_MATERIAL(mname) public: static const char* material_name() {return mname;} private: const char* name_impl() const { return mname; }

#define MATERIAL_UPDATE_WITH_COMMAND(context, scene_context, render_context, update_method, command)    \
    for (size_t i = 0; i < render_context.nodes_number; ++i)                \
    {                                                                       \
        for (size_t j = 0; j < render_context.nodes[i].mesh.instance_parts.size(); ++j) \
        {   \
            MeshPartInstance& part_instance = render_context.nodes[i].mesh.instance_parts[j];   \
            MeshPart& part = render_context.nodes[i].mesh.mesh.parts[j]; \
            if (part_instance.material.material_system != id() || !part_instance.visible)    \
                continue;                                                                           \
            update_method(context, scene_context, render_context, &part_instance, &part, 1); \
            setup_draw_call(render_context.draw_calls.add(), part_instance, render_context.nodes[i].mesh.mesh.parts[j], command);   \
        }   \
    }

#define DEFAULT_MATERIAL_UPDATE(context, scene_context, render_context)     \
    MATERIAL_UPDATE_WITH_COMMAND(context, scene_context, render_context, update, nullptr)

}

#endif
