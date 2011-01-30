#include "gui/mhe_label.hpp"
#include "mhe_gl.hpp"

namespace mhe {
namespace gui {

    void mheLabel::draw_impl()
    {
        glColor3fv(color_.get());
        glBegin(GL_QUADS);
        glVertex2f(g.ll().x(), g.ll().y());
        glVertex2f(g.ll().x(), g.rh().y());
        glVertex2f(g.rh().x(), g.rh().y());
        glVertex2f(g.rh().x(), g.ll().y());
        glEnd();
        fnt->print(g.ll().x(), g.ll().y() + (static_cast<int>(g.height()) >> 1), caption_);
    }
};
};
