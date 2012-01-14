#ifndef __ASPECT_EVENT_TYPES_HPP__
#define __ASPECT_EVENT_TYPES_HPP__

namespace mhe {
namespace game {

enum
{
	tick_event,        	// 0
	draw_event,         // 1
	mouse_on_event,     // 2
	mouse_left_event,   // 3
	mouse_click_event,  // 4
	mouse_release_event, // 5
	mouse_move_int_event,
	enable_event,
	disable_event,
	transform_event,
	start_animation_event,
	stop_animation_event,
	begin_event,
	end_event,
	create_event,
	destroy_event
	
	// user aspect types began from 1000
};

}}

#endif
