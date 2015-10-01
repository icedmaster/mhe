#include "scene/camera_controller.hpp"

#include "scene/scene.hpp"

namespace mhe {

CameraController::CameraController(Scene& scene) :
	scene_(scene),
	move_speed_(20.0f),
	rotation_speed_(5.0f)
{
	camera_node_.transform_id = scene_.transform_pool().create();
}

TransformInstance& CameraController::transform_instance()
{
	return scene_.transform_pool().get(camera_node_.transform_id);
}

void CameraController::sync_camera()
{
	camera_node_.camera.set(transform_instance().transform.position(), transform_instance().transform.rotation());
}

}
