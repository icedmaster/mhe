#ifndef __CAMERA_CONTROLLER_HPP__
#define __CAMERA_CONTROLLER_HPP__

#include "core/ref_counter.hpp"
#include "camera.hpp"

namespace mhe {

    struct RenderContext;
    class Scene;

    class MHE_EXPORT CameraController : public ref_counter
    {
    public:
        CameraController(Scene& scene);

        virtual ~CameraController() {}

        void update(const RenderContext& render_context)
        {
            update_impl(render_context);
        }

        const Camera& camera() const
        {
            return camera_node_.camera;
        }

        Camera& camera()
        {
            return camera_node_.camera;
        }

        TransformInstance& transform_instance();

        void set_move_speed(float speed)
        {
            move_speed_ = speed;
        }

        void set_rotation_speed(float speed)
        {
            rotation_speed_ = speed;
        }
    protected:
        float move_speed() const
        {
            return move_speed_;
        }

        float rotation_speed() const
        {
            return rotation_speed_;
        }

        void sync_camera();
    private:
        virtual void update_impl(const RenderContext& render_context) = 0;

        Scene& scene_;
        CameraNode camera_node_;
        float move_speed_;
        float rotation_speed_;
    };

}

#endif
