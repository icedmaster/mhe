#include "game/engine.hpp"

#include "render/unlit_material_system.hpp"
#include "render/skybox_material_system.hpp"
#include "render/gbuffer_material_system.hpp"
#include "render/posteffect_material_system.hpp"

namespace mhe {
namespace game {

void Engine::setup_generated()
{
	MaterialSystemFactory::instance().add<UnlitMaterialSystem>();
	MaterialSystemFactory::instance().add<SkyboxMaterialSystem>();
	MaterialSystemFactory::instance().add<GBufferFillMaterialSystem>();
	MaterialSystemFactory::instance().add<GBufferDrawMaterialSystem>();
	MaterialSystemFactory::instance().add<PosteffectSimpleMaterialSystem>();
}

}}
