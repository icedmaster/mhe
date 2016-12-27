#include "platform/opengl/opengl_extension.hpp"

#include <iostream>
#include <vector>
#include "utils/global_log.hpp"
#include "utils/strutils.hpp"

namespace mhe {
void OpenGLExtensions::init_extensions()
{
#ifndef MHE_USE_NATIVE_OPENGL
    // multitexture
    glActiveTexture_ = load_extension<PFNGLACTIVETEXTUREARBPROC>("glActiveTexture");
    glClientActiveTexture_ = load_extension<PFNGLCLIENTACTIVETEXTUREARBPROC>("glClientActiveTexture");
    glMultiTexCoord2f_ = load_extension<PFNGLMULTITEXCOORD2FARBPROC>("glMultiTexCoord2f");

#ifdef MHE_OPENGL_HAS_SHADERS
    glCreateProgram_ = load_extension<PFNGLCREATEPROGRAMPROC>("glCreateProgram");
    glDeleteProgram_ = load_extension<PFNGLDELETEPROGRAMPROC>("glDeleteProgram");
    glCreateShader_ = load_extension<PFNGLCREATESHADERPROC>("glCreateShader");
    glDeleteShader_ = load_extension<PFNGLDELETESHADERPROC>("glDeleteShader");
    glShaderSource_ = load_extension<PFNGLSHADERSOURCEPROC>("glShaderSource");
    glCompileShader_ = load_extension<PFNGLCOMPILESHADERPROC>("glCompileShader");
    glAttachShader_ = load_extension<PFNGLATTACHSHADERPROC>("glAttachShader");
    glLinkProgram_ = load_extension<PFNGLLINKPROGRAMPROC>("glLinkProgram");
    glUseProgram_ = load_extension<PFNGLUSEPROGRAMPROC>("glUseProgram");
    glValidateProgram_ = load_extension<PFNGLVALIDATEPROGRAMPROC>("glValidateProgram");
    glGetShaderiv_ = load_extension<PFNGLGETSHADERIVPROC>("glGetShaderiv");
    glGetShaderInfoLog_ = load_extension<PFNGLGETSHADERINFOLOGPROC>("glGetShaderInfoLog");
    glGetProgramiv_ = load_extension<PFNGLGETPROGRAMIVPROC>("glGetProgramiv");
    glGetProgramInfoLog_ = load_extension<PFNGLGETPROGRAMINFOLOGPROC>("glGetProgramInfoLog");
    glGetUniformLocation_ = load_extension<PFNGLGETUNIFORMLOCATIONPROC>("glGetUniformLocation");
    glUniformMatrix4fv_ = load_extension<PFNGLUNIFORMMATRIX4FVPROC>("glUniformMatrix4fv");
    glUniformMatrix3fv_ = load_extension<PFNGLUNIFORMMATRIX3FVPROC>("glUniformMatrix3fv");
    glUniform3fv_ = load_extension<PFNGLUNIFORM3FVPROC>("glUniform3fv");
    glUniform4fv_ = load_extension<PFNGLUNIFORM4FVPROC>("glUniform4fv");
    glUniform1i_ = load_extension<PFNGLUNIFORM1IPROC>("glUniform1i");
    glUniform1f_ = load_extension<PFNGLUNIFORM1FPROC>("glUniform1f");
    glGetAttribLocation_ = load_extension<PFNGLGETATTRIBLOCATIONPROC>("glGetAttribLocation");
    glVertexAttribPointer_ = load_extension<PFNGLVERTEXATTRIBPOINTERPROC>("glVertexAttribPointer");
    glVertexAttribIPointer_ = load_extension<PFNGLVERTEXATTRIBIPOINTERPROC>("glVertexAttribIPointer");
    glEnableVertexAttribArray_ = load_extension<PFNGLENABLEVERTEXATTRIBARRAYPROC>("glEnableVertexAttribArray");
    glDisableVertexAttribArray_ = load_extension<PFNGLDISABLEVERTEXATTRIBARRAYPROC>("glDisableVertexAttribArray");
    glGenBuffers_ = load_extension<PFNGLGENBUFFERSPROC>("glGenBuffers");
    glDeleteBuffers_ = load_extension<PFNGLDELETEBUFFERSPROC>("glDeleteBuffers");
    glBindBuffer_ = load_extension<PFNGLBINDBUFFERPROC>("glBindBuffer");
    glBufferData_ = load_extension<PFNGLBUFFERDATAPROC>("glBufferData");
    glGenVertexArrays_ = load_extension<PFNGLGENVERTEXARRAYSPROC>("glGenVertexArrays");
    glDeleteVertexArrays_ = load_extension<PFNGLDELETEVERTEXARRAYSPROC>("glDeleteVertexArrays");
    glBindVertexArray_ = load_extension<PFNGLBINDVERTEXARRAYPROC>("glBindVertexArray");
    glGetActiveUniform_ = load_extension<PFNGLGETACTIVEUNIFORMPROC>("glGetActiveUniform");
    glGenFramebuffers_ = load_extension<PFNGLGENFRAMEBUFFERSPROC>("glGenFramebuffers");
    glBindFramebuffer_ = load_extension<PFNGLBINDFRAMEBUFFERPROC>("glBindFramebuffer");
    glFramebufferTexture_ = load_extension<PFNGLFRAMEBUFFERTEXTUREPROC>("glFramebufferTexture");
    glGetUniformBlockIndex_ = load_extension<PFNGLGETUNIFORMBLOCKINDEXPROC>("glGetUniformBlockIndex");
    glGetUniformIndices_ = load_extension<PFNGLGETUNIFORMINDICESPROC>("glGetUniformIndices");
    glGetActiveUniformBlockiv_ = load_extension<PFNGLGETACTIVEUNIFORMBLOCKIVPROC>("glGetActiveUniformBlockiv");
    glGetActiveUniformsiv_ = load_extension<PFNGLGETACTIVEUNIFORMSIVPROC>("glGetActiveUniformsiv");
    glBindBufferBase_ = load_extension<PFNGLBINDBUFFERBASEPROC>("glBindBufferBase");
    glBufferSubData_ = load_extension<PFNGLBUFFERSUBDATAPROC>("glBufferSubData");
    glUniformBlockBinding_ = load_extension<PFNGLUNIFORMBLOCKBINDINGPROC>("glUniformBlockBinding");
    glCheckFramebufferStatus_ = load_extension<PFNGLCHECKFRAMEBUFFERSTATUSPROC>("glCheckFramebufferStatus");
    glDrawBuffers_ = load_extension<PFNGLDRAWBUFFERSPROC>("glDrawBuffers");
    glBlendFuncSeparate_ = load_extension<PFNGLBLENDFUNCSEPARATEPROC>("glBlendFuncSeparate");
    glBlendEquation_ = load_extension<PFNGLBLENDEQUATIONPROC>("glBlendEquation");
    glStencilFuncSeparate_ = load_extension<PFNGLSTENCILFUNCSEPARATEPROC>("glStencilFuncSeparate");
    glStencilOpSeparate_ = load_extension<PFNGLSTENCILOPSEPARATEPROC>("glStencilOpSeparate");
    glMapBuffer_ = load_extension<PFNGLMAPBUFFERPROC>("glMapBuffer");
    glUnmapBuffer_ = load_extension<PFNGLUNMAPBUFFERPROC>("glUnmapBuffer");
    glMapBufferRange_ = load_extension<PFNGLMAPBUFFERRANGEPROC>("glMapBufferRange");
    glDrawRangeElements_ = load_extension<PFNGLDRAWRANGEELEMENTSPROC>("glDrawRangeElements");
    glDrawElementsBaseVertex_ = load_extension<PFNGLDRAWELEMENTSBASEVERTEXPROC>("glDrawElementsBaseVertex");
    glGenerateMipmap_ = load_extension<PFNGLGENERATEMIPMAPPROC>("glGenerateMipmap");
    glFramebufferTexture2D_ = load_extension<PFNGLFRAMEBUFFERTEXTURE2DPROC>("glFramebufferTexture2D");
    glDeleteFramebuffers_ = load_extension<PFNGLDELETEFRAMEBUFFERSPROC>("glDeleteFramebuffers");
    glTexBuffer_ = load_extension<PFNGLTEXBUFFERPROC>("glTexBuffer");
    glGenQueries_ = load_extension<PFNGLGENQUERIESPROC>("glGenQueries");
    glDeleteQueries_ = load_extension<PFNGLDELETEQUERIESPROC>("glDeleteQueries");
    glBeginQuery_ = load_extension<PFNGLBEGINQUERYPROC>("glBeginQuery");
    glEndQuery_ = load_extension<PFNGLENDQUERYPROC>("glEndQuery");
    glGetQueryiv_ = load_extension<PFNGLGETQUERYIVPROC>("glGetQueryiv");
    glGetQueryObjectiv_ = load_extension<PFNGLGETQUERYOBJECTIVPROC>("glGetQueryObjectiv");
    glGetQueryObjectui64v_ = load_extension<PFNGLGETQUERYOBJECTUI64VPROC>("glGetQueryObjectui64v");
    glQueryCounter_ = load_extension<PFNGLQUERYCOUNTERPROC>("glQueryCounter");
    glGetBufferSubData_ = load_extension<PFNGLGETBUFFERSUBDATAPROC>("glGetBufferSubData");
    glDispatchCompute_ = load_extension<PFNGLDISPATCHCOMPUTEPROC>("glDispatchCompute");
    glBindImageTexture_ = load_extension<PFNGLBINDIMAGETEXTUREPROC>("glBindImageTexture");
    glMemoryBarrier_ = load_extension<PFNGLMEMORYBARRIERPROC>("glMemoryBarrier");
    glTexImage3D_ = load_extension<PFNGLTEXIMAGE3DPROC>("glTexImage3D");
    glFramebufferTexture3D_ = load_extension<PFNGLFRAMEBUFFERTEXTURE3DPROC>("glFramebufferTexture3D");
#endif  // MHE_OPENGL_HAS_SHADERS

    get_str_extensions();

    INFO_LOG("supported extensions:" << get_supported_extensions());
#endif
}

bool OpenGLExtensions::is_extension_supported(const std::string& ext_name) const
{
#ifndef MHE_USE_NATIVE_OPENGL
    std::map<std::string, bool>::const_iterator it = loaded_extensions_.find(ext_name);
    if (it == loaded_extensions_.end()) return false;
    return it->second;
#else
    UNUSED(ext_name);
    return true;
#endif
}

std::string OpenGLExtensions::get_supported_extensions() const
{
    const char* ext = reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS));
    if (ext == nullptr) return std::string();
    return std::string(ext);
}

void OpenGLExtensions::get_str_extensions()
{
    const std::string& strext = get_supported_extensions();
    std::vector<std::string> ext = utils::split(strext, " ");
    for (size_t i = 0; i < ext.size(); ++i)
        loaded_extensions_[ext[i]] = true;
}

}   // mhe
