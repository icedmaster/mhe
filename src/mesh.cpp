#include "mesh.hpp"

namespace mhe {

void Mesh::draw_impl()
{
	// first, draw all submeshes
	for (size_t i = 0; i < sm_.size(); ++i)
		sm_[i]->draw();

	if (!numfaces)
		return;	// nothing to render

	t_->prepare();
	glVertexPointer(3, GL_FLOAT, 0, &v_[0]);
	glNormalPointer(GL_FLOAT, 0, &n_[0]);
	//t_->render();
	glDrawElements(GL_TRIANGLES, numfaces,
				   GL_UNSIGNED_INT, &i_[0]);
	t_->clean();
}

};
