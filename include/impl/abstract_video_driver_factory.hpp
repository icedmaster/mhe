#ifndef __ABSTRACT_VIDEO_DRIVER_FACTORY_HPP__
#define __ABSTRACT_VIDEO_DRIVER_FACTORY_HPP__

#include <string>
#include "core/ref_counter.hpp"

namespace mhe {

class DriverImpl;
class RenderBufferImpl;
class LayoutImpl;
class Texture;
class ShaderProgram;

class AbstractVideoDriverFactory : public ref_counter
{
public:
	virtual ~AbstractVideoDriverFactory() {}
	
	virtual std::string name() const = 0;

	virtual DriverImpl* create_video_driver() const = 0;
	virtual RenderBufferImpl* create_render_buffer() const = 0;
	virtual LayoutImpl* create_layout() const = 0;
	//virtual Texture* create_texture() const = 0;
	//virtual ShaderProgram* create_shader_program() const = 0;
};

}

#endif
