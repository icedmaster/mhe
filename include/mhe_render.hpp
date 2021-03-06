// This file has been generated by the tools/generate_headers.py script

#ifndef __MHE_RENDER__
#define __MHE_RENDER__

#include "render/animation.hpp"
#include "render/commands.hpp"
#include "render/context.hpp"
#include "render/csm_depth_rendering_material_system.hpp"
#include "render/cubemap_creation_material_system.hpp"
#include "render/deferred_renderer.hpp"
#include "render/definitions.hpp"
#include "render/depth_write_material_system.hpp"
#include "render/draw_call.hpp"
#include "render/gbuffer_helper.hpp"
#include "render/gbuffer_material_system.hpp"
#include "render/image.hpp"
#include "render/initialization.hpp"
#include "render/instances.hpp"
#include "render/layouts.hpp"
#include "render/light.hpp"
#include "render/light_instance_methods.hpp"
#include "render/lpv_material_system.hpp"
#include "render/material.hpp"
#include "render/material_system.hpp"
#include "render/material_systems_types.hpp"
#include "render/mesh.hpp"
#include "render/mesh_grid.hpp"
#include "render/node.hpp"
#include "render/node_instance_methods.hpp"
#include "render/posteffect_material_system.hpp"
#include "render/query.hpp"
#include "render/renderer.hpp"
#include "render/render_buffer.hpp"
#include "render/render_common.hpp"
#include "render/render_context.hpp"
#include "render/render_globals.hpp"
#include "render/render_state.hpp"
#include "render/render_target.hpp"
#include "render/rsm_common.hpp"
#include "render/rsm_material_system.hpp"
#include "render/scene_context.hpp"
#include "render/shader_program.hpp"
#include "render/skybox_material_system.hpp"
#include "render/texture.hpp"
#include "render/uniforms.hpp"
#include "render/unlit_material_system.hpp"
#include "render/video_driver.hpp"
#include "render/view.hpp"
#include "render/window_system.hpp"

#include "render/utils/explicit_draw_call_helper.hpp"
#include "render/utils/simple_meshes.hpp"

#endif // __MHE_RENDER__