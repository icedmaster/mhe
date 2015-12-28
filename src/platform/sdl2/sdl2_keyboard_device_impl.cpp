#include "platform/sdl2/sdl2_keyboard_device_impl.hpp"

#include "events/keyboard_event.hpp"
#include "platform/sdl2/sdl2_event_helper.hpp"

namespace mhe {
namespace sdl {

void SDL2KeyboardDeviceImpl::check(Device::events_vector& events, const WindowSystem& /*ws*/)
{
    SDL_events_vector sdl_events;
    int masks[2] = {SDL_KEYDOWN, SDL_KEYUP};
    int cnt = check_for_events(masks, 2, sdl_events);
    if (cnt <= 0) return;
    events.resize(cnt);
    for (int i = 0; i < cnt; ++i)
        setup_event(static_cast<KeyboardEvent*>(events[i].get()), sdl_events[i]);
}

void SDL2KeyboardDeviceImpl::setup_event(KeyboardEvent* event, const SDL_Event& sdl_event) const
{
    int et = (sdl_event.type == SDL_KEYDOWN) ? KeyboardEvent::key_pressed : KeyboardEvent::key_released;
    int sym = (sdl_event.key.keysym.sym & (1 << 30)) ? (sdl_event.key.keysym.sym & ~(1 << 30)) + 128 : sdl_event.key.keysym.sym;
    return event->setup_event(et, sym);
}

}}
