#ifndef __APPLICATION2D_HPP__
#define __APPLICATION2D_HPP__

#include "application.hpp"

namespace mhe {
namespace app {

class Application2D : public Application
{
public:
	Application2D(const std::string& name) :
		Application(name)
	{}

	virtual ~Application2D() {}
protected:
	virtual bool mhe_app_init(const ApplicationConfig& config,
							  const boost::shared_ptr<game::GameScene>& first_scene);
};

}}

#endif
