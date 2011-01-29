#include "gui/mhe_label.hpp"

namespace mhe {
namespace gui {

    void mheLabel::draw_impl()
    {
        fnt->print(g.ll().x(), g.ll().y(), caption_);
    }
};
};
