#include "gui/label.hpp"

namespace mhe {
namespace gui {

    bool Label::handle_mouse_event(const MouseEvent& me)
    {
        // check mouse position
        if (impl_->geom().in(me.position().x, me.position().y))
        {
            // if button pressed call click handler
            if ( (me.type() != MOUSE_MOVE) && on_mouse_click_ )
                on_mouse_click_->handle(me);
            else if (on_mouse_move_)
                on_mouse_move_->handle(me);
            return true;
        }
        return false;
    }

};
};
