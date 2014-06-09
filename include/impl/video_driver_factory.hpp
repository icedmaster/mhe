#ifndef __VIDEO_DRIVERS_FACTORY_HPP__
#define __VIDEO_DRIVERS_FACTORY_HPP__

#include <vector>
#include "core/ref_ptr.hpp"
#include "core/compiler.hpp"
#include "abstract_video_driver_factory.hpp"

namespace mhe {

class MHE_EXPORT VideoDriverFactory
{
public:
	VideoDriverFactory();

	std::vector<std::string> available_drivers_list() const;
	std::string current_driver_name() const;

	std::string set_next_driver();

	DriverImpl* create_video_driver() const;
	RenderBufferImpl* create_render_buffer() const;
	IndexBufferImpl* create_index_buffer() const;
	LayoutImpl* create_layout() const;
	UniformBufferImpl* create_uniform_buffer() const;
	ShaderProgramImpl* create_shader_program() const;
	TextureImpl* create_texture() const;
private:
	typedef std::vector< ref_ptr<AbstractVideoDriverFactory> > drvvec;
	drvvec drivers_;
	ref_ptr<AbstractVideoDriverFactory> current_driver_factory_;
};

}

#endif
