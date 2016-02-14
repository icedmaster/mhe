#ifndef __OPENGL_TYPES_HPP__
#define __OPENGL_TYPES_HPP__

#include "render/render_buffer.hpp"
#include "render/mesh.hpp"
#include "render/texture.hpp"
#include "render/render_state.hpp"
#include "render/query.hpp"

namespace mhe {
namespace opengl {

const GLuint invalid_unit = static_cast<GLuint>(-1);

inline const char* get_error_desc(GLenum err)
{
    switch (err)
    {
    case GL_NO_ERROR: return "GL_NO_ERROR";
    case GL_INVALID_ENUM: return "GL_INVALID_ENUM";
    case GL_INVALID_VALUE: return "GL_INVALID_VALUE";
    case GL_INVALID_OPERATION: return "GL_INVALID_OPERATION";
    default: NOT_IMPLEMENTED_ASSERT(0, "Error description"); return "";
    }
}

inline GLenum get_vbo_usage(BufferUpdateType type)
{
    switch (type)
    {
        case buffer_update_type_static:
            return GL_STATIC_DRAW;
        case buffer_update_type_dynamic:
            return GL_DYNAMIC_DRAW;
        default:
            ASSERT(0, "Invalid type " << type);
            return GL_STATIC_DRAW;
    }
}

inline GLenum get_primitive_type(Primitive primitive)
{
    ASSERT(primitive < primitive_max, "Invalid primitive:" << primitive);
    GLenum types[primitive_max] = {GL_TRIANGLES, GL_LINES, GL_POINTS, GL_POINTS};

    return types[primitive];
}

inline GLenum get_texture_target(int type)
{
    ASSERT(type < 3, "Invalid texture type");
    GLenum targets[3] = {GL_TEXTURE_2D, GL_TEXTURE_CUBE_MAP, GL_TEXTURE_3D};
    return targets[type];
}

inline GLenum get_texture_filter(int filter)
{
    ASSERT(filter < 6, "Invalid texture filter");
    GLenum filters[6] = {GL_NEAREST, GL_LINEAR, GL_NEAREST_MIPMAP_NEAREST, GL_LINEAR_MIPMAP_NEAREST, GL_NEAREST_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR};
    return filters[filter];
}

inline GLenum get_texture_address_mode(int mode)
{
    ASSERT(mode < 3, "Invalid texture address mode");
    GLenum modes[3] = {GL_REPEAT, GL_CLAMP, GL_CLAMP_TO_EDGE};
    return modes[mode];
}

inline GLenum get_format(int format)
{
    ASSERT(format < format_max, "Invalid format");
    GLenum formats[format_max] = {GL_RGBA, GL_BGRA, GL_RGBA32F, GL_DEPTH_STENCIL, GL_DEPTH_COMPONENT24, GL_R32F, GL_R16F, GL_RGB, GL_RG16F, GL_RGBA16F, GL_RGB32F, GL_RGB16F};
    return formats[format];
}

// Formats supported by glBindImageTexture()
inline GLenum get_image_format(int format)
{
    ASSERT(format < format_max, "Invalid format");
    GLenum formats[format_max] = {GL_RGBA8, GL_RGBA8, GL_RGBA32F, GL_R32F, GL_R32F, GL_R32F, GL_R16F, GL_RGB, GL_RG16F, GL_RGBA16F, GL_RGB32F, GL_RGB16F};
    return formats[format];
}

inline GLenum get_pixel_data_format(int format)
{
    ASSERT(format < format_max, "Invalid texture format");
    GLenum formats[format_max] = {GL_RGBA, GL_BGRA, GL_RGBA, GL_DEPTH_STENCIL, GL_DEPTH_COMPONENT, GL_RED, GL_RED, GL_RGB, GL_RG, GL_RGBA, GL_RGB, GL_RGB};
    return formats[format];
}

inline GLenum get_datatype(int type)
{
    ASSERT(type < 5, "Invalid texture datatype");
    ASSERT(type != format_default, "You must use method get_datatype_by_format()");
    GLenum types[5] = {GL_UNSIGNED_BYTE, GL_FLOAT, GL_UNSIGNED_INT, GL_INT, GL_UNSIGNED_BYTE};
    return types[type];
}

inline GLenum get_datatype_by_format(int format)
{
    ASSERT(format < format_max, "Invalid texture format");
    GLenum types[format_max] = {GL_UNSIGNED_BYTE, GL_UNSIGNED_BYTE, GL_FLOAT, GL_UNSIGNED_BYTE, GL_UNSIGNED_BYTE,
        GL_FLOAT, GL_FLOAT, GL_UNSIGNED_BYTE, GL_FLOAT, GL_FLOAT, GL_FLOAT, GL_FLOAT};
    return types[format];
}

inline GLenum get_framebuffer_format(int format)
{
    ASSERT(format < 3, "Invalid framebuffer format");
    GLenum formats[3] = {GL_READ_FRAMEBUFFER, GL_DRAW_FRAMEBUFFER, GL_FRAMEBUFFER};
    return formats[format];
}

inline GLenum get_blend_operation(BlendFunc func)
{
    ASSERT(func < blend_func_max, "Invalid blend func");
    GLenum funcs[blend_func_max] = {GL_FUNC_ADD, GL_MULT, GL_MIN, GL_MAX};
    return funcs[func];
}

inline GLenum get_blend_mode(BlendMode mode)
{
    ASSERT(mode < 4, "Invalid blend mode");
    GLenum modes[4] = {GL_ZERO, GL_ONE, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA};
    return modes[mode];
}

inline GLenum get_stencil_op(StencilOp op)
{
    ASSERT(op < 2, "Invalid stencil op");
    GLenum ops[2] = {GL_KEEP, GL_REPLACE};
    return ops[op];
}

inline GLenum get_compare_mode(CompareMode mode)
{
    ASSERT(mode < 3, "Invalid compare mode");
    GLenum modes[3] = {GL_ALWAYS, GL_EQUAL, GL_NOTEQUAL};
    return modes[mode];
}

inline size_t get_bytes_per_format(GLenum format)
{
    switch (format)
    {
    case GL_RGBA:
        return 4;
    case GL_RGBA16F:
        return 4 * 2;
    case GL_RGBA32F:
        return 4 * 4;
    case GL_RGB32F:
        return 3 * 4;
    default:
        ASSERT(false, "Invalid format:" << format);
        break;
    }
    return 0;
}

inline GLenum get_cull_mode(CullMode mode)
{
    ASSERT(mode < 4, "Invalid mode");
    GLenum modes[4] = {GL_NONE, GL_FRONT, GL_BACK, GL_FRONT_AND_BACK};
    return modes[mode];
}

inline GLenum get_winding_order(WindingOrder order)
{
    ASSERT(order < 3, "Invalid order");
    GLenum orders[2] = {GL_CW, GL_CCW};
    return orders[order];
}

inline GLenum get_side(int side)
{
    ASSERT(side < 6, "Invalid side");
    GLenum sides[6] = {GL_TEXTURE_CUBE_MAP_POSITIVE_X, GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
        GL_TEXTURE_CUBE_MAP_POSITIVE_Y, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
        GL_TEXTURE_CUBE_MAP_POSITIVE_Z, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z};
    return sides[side];
}

inline GLenum get_side_offset(int side)
{
    return get_side(side) - GL_TEXTURE_CUBE_MAP_POSITIVE_X;
}

inline GLenum get_query_target(int target)
{
    ASSERT(target < query_max, "invalid target");
    GLenum targets[query_max] = {GL_TIMESTAMP, GL_TIME_ELAPSED};
    return targets[target];
}

inline GLenum get_access(int access)
{
    ASSERT(access < access_max, "invalid access");
    GLenum gl_access[access_max] = {GL_READ_WRITE, GL_READ_ONLY, GL_WRITE_ONLY};
    return gl_access[access];
}

inline GLbitfield get_barriers(uint32_t barriers)
{
    if (barriers == memory_barrier_all)
        return GL_ALL_BARRIER_BITS;
    GLbitfield result = 0;
    // TODO: need to optimize this method
    if (barriers & memory_barrier_storage_buffer)
        result |= GL_SHADER_STORAGE_BARRIER_BIT;
    return result;
}

}}

#endif
