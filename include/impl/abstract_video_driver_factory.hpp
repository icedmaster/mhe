#ifndef __ABSTRACT_VIDEO_DRIVER_FACTORY_HPP__
#define __ABSTRACT_VIDEO_DRIVER_FACTORY_HPP__

#include <string>
#include "core/ref_counter.hpp"

namespace mhe {

class DriverImpl;
class RenderBufferImpl;
class IndexBufferImpl;
class LayoutImpl;
class TextureImpl;
class ShaderProgramImpl;
class UniformBufferImpl;

class AbstractVideoDriverFactory : public ref_counter
{
public:
	virtual ~AbstractVideoDriverFactory() {}
	
	virtual std::string name() const = 0;

	virtual DriverImpl* create_video_driver() const = 0;
	virtual RenderBufferImpl* create_render_buffer() const = 0;
	virtual IndexBufferImpl* create_index_buffer() const = 0;
	virtual LayoutImpl* create_layout() const = 0;
	virtual UniformBufferImpl* create_uniform_buffer() const = 0;
	virtual ShaderProgramImpl* create_shader_program() const = 0;
	virtual TextureImpl* create_texture() const = 0;
};

}

#endif
