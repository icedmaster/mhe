#include "app/application2d.hpp"

#include "camera2d.hpp"

namespace mhe {

bool Application2D::mhe_app_init(const ApplicationConfig& config)
{
	if (!Application::mhe_app_init(config))
		return false;
	// init default 2d camera
	const vector2<int>& size = engine().context().window_system().screen_size();
	engine().game_scene()->scene()->add_camera(new Camera2D(size.x(), size.y()));
	return true;
}

}
