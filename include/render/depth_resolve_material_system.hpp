#ifndef __DEPTH_RESOLVE_MATERIAL_SYSTEM_HPP__
#define __DEPTH_RESOLVE_MATERIAL_SYSTEM_HPP__

#include "posteffect_material_system.hpp"

namespace mhe {

// TODO: will linearize depth
class DepthResolveMaterialSystem : public PosteffectMaterialSystemBase
{
    SETUP_POSTEFFECT_MATERIAL(DepthResolveMaterialSystem, "depth_resolve")
};

}

#endif
