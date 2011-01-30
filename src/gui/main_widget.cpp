#include "gui/main_widget.hpp"

namespace mhe {
namespace gui {

    void MainWidget::add_widget(const boost::shared_ptr<Widget>& w)
    {
        childs_.insert(wmap::value_type(w->name(), w));
    }

    void MainWidget::draw_impl()
    {
        for (wmap::const_iterator it = childs_.begin();
                                  it != childs_.end();
                                  ++it)
            it->second->draw();
    }

    bool MainWidget::handle_keyboard_event(const KeyboardEvent& ke)
    {
        for (wmap::const_iterator it = childs_.begin();
                                  it != childs_.end();
                                  ++it)
        {
            if (it->second->handleKeyboardEvent(ke))
                return true;
        }
        return false;
    }

    bool MainWidget::handle_mouse_event(const MouseEvent& me)
    {
        for (wmap::const_iterator it = childs_.begin();
                                  it != childs_.end();
                                  ++it)
        {
            if (it->second->handleMouseEvent(me))
                return true;
        }
        return false;
    }
};
};
