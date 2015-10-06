#ifndef __OPENGLES_VIDEO_DRIVER_FACTORY_HPP__
#define __OPENGLES_VIDEO_DRIVER_FACTORY_HPP__

#include "impl/abstract_video_driver_factory.hpp"
#include "core/types.hpp"
#include "opengl3_driver.hpp"
#include "opengl3_buffer.hpp"
#include "opengl3_shader_program.hpp"
#include "opengl3_texture.hpp"
#include "opengl3_render_state.hpp"
#include "opengl3_render_target.hpp"
#include "opengl3_query.hpp"

namespace mhe {
namespace opengl {

class OpenGL3VideoDriverFactory : public AbstractVideoDriverFactory
{
public:
	std::string name() const
	{
		return "opengl3";
	}

	DriverImpl* create_video_driver() const
	{
		return new OpenGL3Driver;
	}

	RenderBufferImpl* create_render_buffer() const
	{
		return new OpenGL3Buffer;
	}

	IndexBufferImpl* create_index_buffer() const
	{
		return new OpenGL3IndexBuffer;
	}

	LayoutImpl* create_layout() const
	{
		return new OpenGL3Layout;
	}

	UniformBufferImpl* create_uniform_buffer() const
	{
		return new OpenGL3UniformBuffer;
	}

	ShaderProgramImpl* create_shader_program() const
	{
		return new OpenGL3ShaderProgram;
	}

	TextureImpl* create_texture() const
	{
		return new OpenGL3Texture;
	}

	RenderStateImpl* create_render_state() const
	{
		return new OpenGL3RenderState;
	}

	RenderTargetImpl* create_render_target() const
	{
		return new OpenGL3RenderTarget;
	}

	TextureBufferImpl* create_texture_buffer() const
	{
		return new OpenGL3TextureBuffer;
	}

	QueryImpl* create_query() const override
	{
		return new OpenGL3Query;
	}
};

}}

#endif
