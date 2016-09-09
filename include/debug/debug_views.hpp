#ifndef __DEBUG_VIEWS_HPP__
#define __DEBUG_VIEWS_HPP__

#include "custom_types.hpp"
#include "imgui_helper.hpp"
#include "core/array.hpp"
#include "core/ref_ptr.hpp"
#include "render/render_common.hpp"

namespace mhe {

class EventManager;

namespace game {

class Engine;

}

class DebugViews
{
    struct DebugField : public ref_counter
    {
        virtual ~DebugField() {}
        virtual void update() = 0;
    };

    struct FloatDebugField : public DebugField
    {
        string name;
        float min_value;
        float max_value;
        float* value;

        void update() override;
    };

    struct DebugBuffer
    {
        string name;
        TextureInstance texture;
        int mode;
    };
public:
    class DebugView
    {
    public:
        void set_name(const string& name)
        {
            name_ = name;
        }

        void add(const string& name, float min_value, float max_value, float* value);
        void update();
    private:
        string name_;
        fixed_size_vector< ref_ptr<DebugField>, 16 > fields_;
    };
public:
    DebugViews(game::Engine& engine) :
        engine_(engine)
    {}

    void init(EventManager& event_maneger);
    void destroy();
    void update();
    void render();

    size_t add_view(const string& name);
    DebugView& get_view(size_t id);

    size_t add_debug_buffer(const string& name, const TextureInstance& texture, int debug_mode);

    ImGuiHelper& imgui_helper()
    {
        return imgui_;
    }
private:
    game::Engine& engine_;
    ImGuiHelper imgui_;
    int debug_buffer_;
    fixed_size_vector<DebugView, 16> debug_views_;
    fixed_size_vector<DebugBuffer, 16> debug_buffers_;
    bool stats_enabled_;
};

}

#endif
