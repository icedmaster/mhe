#include "render3d.hpp"
#include "gl_state_attr.hpp"

namespace mhe
{
	void Render3D::set_active_camera(const std::string& name)
	{
		for (size_t i = 0; i < cameras_.size(); ++i)
		{
			if (cameras_[i]->name() == name)
			{
				active_camera_ = cameras_[i];
				active_camera_->setProjection(ws_->width(), ws_->height());
			}
		}
	}

	void Render3D::init_impl()
	{
		GLStateAttr::instance().set(GL_DEPTH);
		GLStateAttr::instance().set(GL_DEPTH_TEST);
		active_camera_->setProjection(ws_->width(), ws_->height());
	}

	void Render3D::save_impl()
	{
		save_attributes();
		glPushMatrix();
		init_impl();
	}

	void Render3D::restore_impl()
	{
		glPopMatrix();
		restore_attributes();
	}

	void Render3D::save_attributes()
	{
		GLStateAttr::instance().save();
	}

	void Render3D::restore_attributes()
	{
		GLStateAttr::instance().restore();
	}

	void Render3D::enable_blend()
	{
		GLStateAttr::instance().set(GL_BLEND);
	}

	void Render3D::disable_blend()
	{
		GLStateAttr::instance().clr(GL_BLEND);
	}

	void Render3D::enable_lighting()
	{
		GLStateAttr::instance().set(GL_LIGHTING);
	}

	void Render3D::disable_lighting()
	{
		GLStateAttr::instance().clr(GL_LIGHTING);
	}

	void Render3D::draw_impl()
	{
		active_camera_->update();
		// draw scene first...
		//scene_->draw();
		// then our subrenders works
		for (size_t i = 0; i < subrenders_.size(); ++i)
		{
			subrenders_[i]->save();
			subrenders_[i]->draw();
			subrenders_[i]->restore();
		}
	}

	void Render3D::render_impl()
	{
		// clear buffer
		clear_impl();
		// draw scene
		draw_impl();
		// update buffers
		ws_->swapBuffers();
	}
};
