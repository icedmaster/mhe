#include "game/engine.hpp"

#include "render/unlit_material_system.hpp"
#include "render/skybox_material_system.hpp"
#include "render/gbuffer_material_system.hpp"
#include "render/posteffect_material_system.hpp"
#include "render/depth_write_material_system.hpp"
#include "render/csm_depth_rendering_material_system.hpp"

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
}

}}
