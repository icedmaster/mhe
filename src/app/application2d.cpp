#include "app/application2d.hpp"

#include "camera2d.hpp"

namespace mhe {
namespace app {

int Application2D::run_impl()
{
	if (engine().game_scene() != nullptr)
	{
		// init default 2d camera
		const vector2<int>& size = engine().context().window_system().screen_size();
		engine().game_scene()->scene()->add_camera(new Camera2D(size.x(), size.y()), true);
    }
	return Application::run_impl();
}

}}
