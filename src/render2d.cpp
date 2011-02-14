#include "render2d.hpp"
#include "gl_state_attr.hpp"

namespace mhe
{
	void Render2D::init_impl()
	{
		glMatrixMode(GL_PROJECTION);
		glOrtho(0, ws_->width(), 0, ws_->height(), -1, 1);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
	}

	void Render2D::save_impl()
	{
		save_attributes();
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		//glLoadIdentity();
		//glOrtho(0, ws_->width(), 0, ws_->height(), -1, 1);
		//glMatrixMode(GL_MODELVIEW);
		//glLoadIdentity();
		//init_impl();
	}

	void Render2D::restore_impl()
	{
	    glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		restore_attributes();
	}

	void Render2D::save_attributes()
	{
		GLStateAttr::instance().save();

		GLStateAttr::instance().clr(GL_DEPTH_TEST);
		GLStateAttr::instance().clr(GL_DEPTH);
	}

	void Render2D::restore_attributes()
	{
		GLStateAttr::instance().restore();
	}

	void Render2D::enable_blend()
	{
		GLStateAttr::instance().set(GL_BLEND);
	}

	void Render2D::disable_blend()
	{
		GLStateAttr::instance().clr(GL_BLEND);
	}

	void Render2D::enable_lighting()
	{
		GLStateAttr::instance().set(GL_LIGHTING);
	}

	void Render2D::disable_lighting()
	{
		GLStateAttr::instance().clr(GL_LIGHTING);
	}

	void Render2D::draw_impl()
	{
		// draw scene first...
		for (size_t i = 0; i < scenes_.size(); ++i)
		{
		    glMatrixMode(GL_PROJECTION);
		    glLoadIdentity();
		    glOrtho(0, scenes_[i]->getViewport()->w(),
                    0, scenes_[i]->getViewport()->h(),
                    -1, 1);
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
			scenes_[i]->draw();
		}
		// then our subrenders works
		for (size_t i = 0; i < subrenders_.size(); ++i)
		{
			subrenders_[i]->save();
			subrenders_[i]->draw();
			subrenders_[i]->restore();
		}
	}

	void Render2D::render_impl()
	{
		// clear buffer
		clear_impl();
		// draw scene
		draw_impl();
		// update buffers
		ws_->swapBuffers();
	}

	void Render2D::add_scene(Scene* scene)
	{
		scenes_.push_back(boost::shared_ptr<Scene>(scene));
	}
};
