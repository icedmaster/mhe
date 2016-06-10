#include "game/engine.hpp"

#include "render/unlit_material_system.hpp"
#include "render/skybox_material_system.hpp"
#include "render/gbuffer_material_system.hpp"
#include "render/posteffect_material_system.hpp"
#include "render/depth_write_material_system.hpp"
#include "render/csm_depth_rendering_material_system.hpp"
#include "render/rsm_material_system.hpp"
#include "render/lpv_material_system.hpp"
#include "render/fog.hpp"

namespace mhe {
namespace game {

void Engine::setup_generated()
{
    MaterialSystemFactory::instance().add<UnlitMaterialSystem>();
    MaterialSystemFactory::instance().add<SkyboxMaterialSystem>();
    MaterialSystemFactory::instance().add<GBufferFillMaterialSystem>();
    MaterialSystemFactory::instance().add<GBufferDrawMaterialSystem>();
    MaterialSystemFactory::instance().add<PosteffectSimpleMaterialSystem>();
    MaterialSystemFactory::instance().add<PosteffectDebugMaterialSystem>();
    MaterialSystemFactory::instance().add<TonemapMaterialSystem>();
    MaterialSystemFactory::instance().add<DepthWriteMaterialSystem>();
    MaterialSystemFactory::instance().add<CSMDepthRenderingMaterialSystem>();
    MaterialSystemFactory::instance().add<FXAAMaterialSystem>();
    MaterialSystemFactory::instance().add<SSRMaterialSystem>();
    MaterialSystemFactory::instance().add<BlurMaterialSystem>();
    MaterialSystemFactory::instance().add<DOFMaterialSystem>();
    MaterialSystemFactory::instance().add<SSAOMaterialSystem>();
    MaterialSystemFactory::instance().add<CompositeMulMaterialSystem>();
    MaterialSystemFactory::instance().add<CompositeAddMaterialSystem>();
    MaterialSystemFactory::instance().add<ProbesAccumulatorMaterialSystem>();
    MaterialSystemFactory::instance().add<AverageLuminanceMaterialSystem>();
    MaterialSystemFactory::instance().add<BloomMaterialSystem>();
    MaterialSystemFactory::instance().add<RSMMaterialSystem>();
    MaterialSystemFactory::instance().add<LPVMaterialSystem>();
    MaterialSystemFactory::instance().add<LPVResolveMaterialSystem>();
    MaterialSystemFactory::instance().add<HeightFogMaterialSystem>();
    MaterialSystemFactory::instance().add<ExponentialShadowMap>();
    MaterialSystemFactory::instance().add<VolumetricFogMaterialSystem>();
    MaterialSystemFactory::instance().add<VolumetricFogResolveMaterialSystem>();
    MaterialSystemFactory::instance().add<VolumetricFogSystem>();
}

}}
