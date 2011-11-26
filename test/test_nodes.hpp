#ifndef __TEST_NODES_HPP__
#define __TEST_NODES_HPP__

#include "inode.hpp"
#include "mhe_gl.hpp"

namespace mhe {
namespace test {

class Axis : public iNode
{
private:
	v3d center;
private:
	void draw_impl(const boost::shared_ptr<iDriver>&/* driver*/)
	{
		// simple OPENGL drawing
		glColor3f(1.0, 0.0, 0.0);
		glBegin(GL_LINES);
		glVertex3fv(center.get());
		glVertex3f(center.x() + 100.0, 0.0, 0.0);
		glEnd();

		glColor3f(0.0, 1.0, 0.0);
		glBegin(GL_LINES);
		glVertex3fv(center.get());
		glVertex3f(0.0, center.y() + 100.0, 0.0);
		glEnd();

		glColor3f(0.0, 0.0, 1.0);
		glBegin(GL_LINES);
		glVertex3fv(center.get());
		glVertex3f(0.0, 0.0, center.z() + 100.0);
		glEnd();

		glColor3f(1.0, 1.0, 1.0);
	}
public:
	void set_center(const v3d& value)
	{
		center = value;
	}
};

class Quad : public iNode
{
private:
	colorf color_;
	float size_;
	v3d pos_;
public:
	Quad() : color_(cfWhite), size_(50.0)  {}

	void set_color(const colorf& color)
	{
		color_ = color;
	}

	void set_size(float size)
	{
		size_ = size;
	}

	void set_position(const v3d& pos)
	{
		pos_ = pos;
	}

private:
	void draw_impl(const boost::shared_ptr<iDriver>& driver)
	{
        // prepare buffers for drawing
        const float v[] = {0.0, 0.0, 0.0,
                           0.0, size_, 0.0,
                           size_, size_, 0.0,
                           size_, 0.0, 0.0};
        const float n[] = {0.0, 0.0, 1.0,
                           0.0, 0.0, 1.0,
                           0.0, 0.0, 1.0,
                           0.0, 0.0, 1.0};
        //const float t[] = {0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0, 0.0};
        const float c[] = {color_.r(), color_.g(), color_.b(), color_.a(),
                           color_.r(), color_.g(), color_.b(), color_.a(),
                           color_.r(), color_.g(), color_.b(), color_.a(),
                           color_.r(), color_.g(), color_.b(), color_.a()};
        const cmn::uint i[] = {0, 1, 2, 2, 3, 0};

		matrixf m;
		m.set_translate(pos_);
		driver->draw(m, v, n, 0, c, i, 6);
	}
};

}}	// namespaces

#endif
