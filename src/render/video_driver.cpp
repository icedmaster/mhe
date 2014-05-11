#include "render/video_driver.hpp"

#include "impl/system_factory.hpp"

namespace mhe {

// helper stats class
void Driver::Stats::update()
{
	++batches_;
}

void Driver::Stats::update_frames()
{
	++frames_;
}

Driver::Driver() :
	impl_(SystemFactory::instance().create_driver())
{
}

void Driver::reset()
{
	stats_.reset();
	impl_.reset(SystemFactory::instance().create_driver());
}

}
