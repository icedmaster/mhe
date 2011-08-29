#ifndef __TEST_NODES_HPP__
#define __TEST_NODES_HPP__

#include "inode.hpp"
#include "mhe_gl.hpp"

namespace mhe {
namespace test {

class Axis : public iNode
{
private:
	void draw_impl(const boost::shared_ptr<iDriver>&/* driver*/)
	{
		// simple OPENGL drawing
		glColor3f(1.0, 0.0, 0.0);
		glBegin(GL_LINES);
		glVertex3f(0.0, 0.0, 0.0);
		glVertex3f(10.0, 0.0, 0.0);
		glEnd();

		glColor3f(0.0, 1.0, 0.0);
		glBegin(GL_LINES);
		glVertex3f(0.0, 0.0, 0.0);
		glVertex3f(0.0, 10.0, 0.0);
		glEnd();

		glColor3f(0.0, 0.0, 1.0);
		glBegin(GL_LINES);
		glVertex3f(0.0, 0.0, 0.0);
		glVertex3f(0.0, 0.0, 10.0);
		glEnd();

		glColor3f(1.0, 1.0, 1.0);
	}
};

}}	// namespaces

#endif
