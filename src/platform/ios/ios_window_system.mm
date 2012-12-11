#include "platform/ios/ios_window_system.hpp"

#include "utils/global_log.hpp"
#include "platform/ios/ios_window_system_impl.hpp"

namespace mhe {
namespace ios {

iOSWindowSystem::iOSWindowSystem() :
	impl_(new iOSWindowSystemImpl)
{}

bool iOSWindowSystem::init(const vector2<int>& r, int bpp, bool fullscreen)
{
	return impl_->init(r, bpp, fullscreen);
}

void iOSWindowSystem::close()
{
	impl_->close();
}

void iOSWindowSystem::swap_buffers()
{
    impl_->swap_buffers();
}

const Surface* iOSWindowSystem::surface() const
{
	return impl_->surface();
}

// implementation



}}
