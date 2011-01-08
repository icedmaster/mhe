#include "material.hpp"
#include "mhe_gl.hpp"

using namespace mhe;

Material::Material() :
	diffuse_front_and_back(false),
	diffuse_color_front(1.0, 1.0, 1.0, 0.0),
	diffuse_color_back(1.0, 1.0, 1.0, 0.0)
{}


Material::Material(const Material& m) :
	diffuse_front_and_back(m.diffuse_front_and_back),
	diffuse_color_front(m.diffuse_color_front),
	diffuse_color_back(m.diffuse_color_back)
{}


void Material::update_impl()
{
	if (diffuse_front_and_back)
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse_color_front.get());
	else
	{
		glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_color_front.get());
		glMaterialfv(GL_BACK, GL_DIFFUSE, diffuse_color_back.get());
	}
}
