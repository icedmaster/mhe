#ifndef __GI_HPP__
#define __GI_HPP__

#include "render_common.hpp"
#include "core/string.hpp"

namespace mhe {

struct Context;
class ShaderStorageBuffer;
class Texture;

class CubemapIntegrator
{
public:
    struct Settings
    {
        string shader_name;
    };

    bool init(Context& context, const Settings& settings);
    bool integrate(ShaderStorageBuffer& dst, Context& context, Texture& cubemap);
private:
    UbershaderHandleType ubershader_id_;
    UniformBufferHandleType settings_uniform_id_;
};

}

#endif
