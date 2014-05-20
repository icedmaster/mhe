#include "platform/opengl3/opengl3_buffer.hpp"

#include "platform/opengl/opengl_types.hpp"
#include "platform/opengl/opengl_extension.hpp"

namespace mhe {
namespace opengl {

bool VAO::init()
{
	OpenGLExtensions::instance().glGenVertexArrays(1, &id_);
    return true;
}

void VAO::close()
{
	OpenGLExtensions::instance().glDeleteVertexArrays(1, &id_);
}

void VAO::enable()
{
	OpenGLExtensions::instance().glBindVertexArray(id_);
}

void VAO::disable()
{
	OpenGLExtensions::instance().glBindVertexArray(0);
}

bool VBO::init(GLenum target, GLsizeiptr size, const GLvoid* data, GLenum usage)
{
    OpenGLExtensions::instance().glGenBuffers(1, &id_);
	if (!id_) return false;
	OpenGLExtensions::instance().glBindBuffer(target, id_);
	OpenGLExtensions::instance().glBufferData(target, size, data, usage);
	target_ = target;
	return true;
}

void VBO::close()
{
	OpenGLExtensions::instance().glDeleteBuffers(1, &id_);
}

void VBO::enable()
{
	OpenGLExtensions::instance().glBindBuffer(target_, id_);
}

void VBO::disable()
{
	OpenGLExtensions::instance().glBindBuffer(target_, 0);
}

bool OpenGL3Buffer::init(BufferUpdateType type, const uint8_t* data, size_t size, size_t /*element_size*/)
{
	if (!vao_.init())
		return false;
	vao_.enable();
	bool result = vbo_.init(GL_ARRAY_BUFFER, size, data, get_vbo_usage(type));
	vao_.disable();
	return result;
}

void OpenGL3Buffer::close()
{
	vbo_.close();
	vao_.close();
}

bool OpenGL3Layout::init(const LayoutDesc& desc)
{
    ASSERT(!desc.layout.empty(), "Getting layout automatically does not supported yet");
	desc_ = desc;
	return true;
}

void OpenGL3Layout::enable()
{
	for (size_t i = 0; i < desc_.layout.size(); ++i)
	{
		const LayoutElement& element = desc_.layout[i];
		OpenGLExtensions::instance().glVertexAttribPointer(element.position, element.size, GL_FLOAT, GL_FALSE,
														   element.stride, reinterpret_cast<const void*>(element.offset));
		OpenGLExtensions::instance().glEnableVertexAttribArray(element.position);
	}
}

void OpenGL3Layout::disable()
{
	for (size_t i = 0; i < desc_.layout.size(); ++i)
	{
		const LayoutElement& element = desc_.layout[i];
		OpenGLExtensions::instance().glDisableVertexAttribArray(element.position);
	}
}

}}
