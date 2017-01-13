#include "debug/debug_views.hpp"

#include "events/event_manager.hpp"
#include "events/keyboard_device.hpp"
#include "events/keyboard_event.hpp"

#include "render/renderer.hpp"
#include "game/engine.hpp"

#include "imgui/imgui.h"

namespace mhe {

void DebugViews::init(EventManager& event_manager)
{
    event_manager.add_bind("standart_stats", keyboard_event_type, KeyboardEvent::key_pressed, KeyboardDevice::key_f3);

    stats_enabled_ = false;
    debug_buffer_ = 0;

    DebugVisualization& buffer = debug_visualizers_.add();
    buffer.mode = Renderer::renderer_debug_mode_none;
    buffer.name = "none";

    imgui_.init(&engine_);
}

void DebugViews::destroy()
{
    imgui_.destroy();
}

void DebugViews::update()
{
    Renderer* renderer = engine_.renderer();

    if (engine_.event_manager().check_bind("standart_stats"))
        stats_enabled_ ^= 1;

    imgui_.update(engine_.context(), engine_.render_context(), engine_.event_manager());

    int current_buffer = debug_buffer_;

    if (stats_enabled_)
    {
        fixed_size_vector<const char*, 16> items(debug_visualizers_.size());
        for (size_t i = 0, size = items.size(); i < size; ++i)
            items[i] = debug_visualizers_[i].name.c_str();
        ImGui::Combo("Debug modes", &current_buffer, &items[0], items.size());
    }

    if (current_buffer != debug_buffer_)
    {
        renderer->set_debug_buffer(static_cast<Renderer::DebugMode>(debug_visualizers_[current_buffer].mode),
            debug_visualizers_[current_buffer].texture);
        if (debug_visualizers_[debug_buffer_].scene != nullptr)
            renderer->remove_scene_entity(debug_visualizers_[debug_buffer_].scene);
        if (debug_visualizers_[current_buffer].scene != nullptr)
            renderer->add_scene_entity(debug_visualizers_[current_buffer].scene);
    }
    debug_buffer_ = current_buffer;

    for (size_t i = 0, size = debug_views_.size(); i < size; ++i)
        debug_views_[i].update();
}

void DebugViews::render()
{
    if (stats_enabled_)
        show_standart_stats_views(engine_.stats());
    imgui_.render(engine_.context(), engine_.render_context());
}

size_t DebugViews::add_view(const string& name)
{
    size_t id = debug_views_.size();
    DebugView& view = debug_views_.add();
    view.set_name(name);
    return id;
}

DebugViews::DebugView& DebugViews::get_view(size_t id)
{
    ASSERT(id < debug_views_.size(), "Invalid debug view id:" << id);
    return debug_views_[id];
}

size_t DebugViews::add_debug_buffer(const string& name, const TextureInstance& texture, int debug_mode)
{
    size_t id = debug_visualizers_.size();
    DebugVisualization& buffer = debug_visualizers_.add();
    buffer.name = name;
    buffer.mode = debug_mode;
    buffer.texture = texture;
    buffer.scene = nullptr;
    return id;
}

size_t DebugViews::add_debug_scene(const string& name, SceneEntity* scene)
{
    ASSERT(scene != nullptr, "Invalid debug scene");
    size_t id = debug_visualizers_.size();
    DebugVisualization& buffer = debug_visualizers_.add();
    buffer.name = name;
    buffer.scene = scene;
    buffer.mode = Renderer::renderer_debug_mode_none;
    return id;
}

void DebugViews::DebugView::add(const string& name, float min_value, float max_value, float* value)
{
    FloatDebugField* field = new FloatDebugField;
    field->name = name;
    field->min_value = min_value;
    field->max_value = max_value;
    field->value = value;

    fields_.push_back(ref_ptr<DebugField>(field));
}

void DebugViews::DebugView::update()
{
    ImGui::Begin(name_.c_str());
    for (size_t i = 0, size = fields_.size(); i < size; ++i)
    {
        fields_[i]->update();
    }
    ImGui::End();
}

void DebugViews::FloatDebugField::update()
{
    ImGui::SliderFloat(name.c_str(), value, min_value, max_value);
}

}
