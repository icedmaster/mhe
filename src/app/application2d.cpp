#include "app/application2d.hpp"

#include "camera2d.hpp"

namespace mhe {
namespace app {

bool Application2D::mhe_app_init(const ApplicationConfig& config,
								 const boost::shared_ptr<game::GameScene>& first_scene)
{
	if (!Application::mhe_app_init(config, first_scene))
		return false;
	if (first_scene == nullptr) return true;
	// init default 2d camera
	const vector2<int>& size = engine().context().window_system().screen_size();
	engine().game_scene()->scene()->add_camera(new Camera2D(size.x(), size.y()));
	return true;
}

}}
