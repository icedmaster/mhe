#include "gui/mhe_label.hpp"
#include "mhe_gl.hpp"
#include "utils/image_loader.hpp"

namespace mhe {
namespace gui {

    void mheLabel::draw_impl()
    {
        if (!texture_)
        {
            glColor3fv(color_.get());
            glBegin(GL_QUADS);
            glVertex2f(g.ll().x(), g.ll().y());
            glVertex2f(g.ll().x(), g.rh().y());
            glVertex2f(g.rh().x(), g.rh().y());
            glVertex2f(g.rh().x(), g.ll().y());
            glEnd();
        }
        else
        {
            // TODO:
            glColor3fv(cfWhite.get());
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, texture_->id());
            glBegin(GL_QUADS);
            texture_->draw_at(0); glVertex2f(g.ll().x(), g.ll().y());
            texture_->draw_at(1); glVertex2f(g.ll().x(), g.rh().y());
            texture_->draw_at(2); glVertex2f(g.rh().x(), g.rh().y());
            texture_->draw_at(3); glVertex2f(g.rh().x(), g.ll().y());
            glEnd();
            glDisable(GL_TEXTURE_2D);
        }
        fnt->print(g.ll().x(), g.ll().y() + (static_cast<int>(g.height()) >> 1), caption_);
    }


    void mheLabel::set_image(const std::string& filename)
    {
        texture_.reset(new Texture);
        Image* im = load_image(filename);
        if (im == nullptr)
            throw mhe::exception("Incorrect image filename: " + filename);
        texture_->setImage(boost::shared_ptr<Image>(im));
        texture_->setCoord(0, v2d(0.0, 0.0));
        texture_->setCoord(1, v2d(0, 1));
        texture_->setCoord(2, v2d(1, 1));
        texture_->setCoord(3, v2d(1, 0));
    }
};
};
