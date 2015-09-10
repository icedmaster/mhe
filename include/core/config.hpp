#ifndef __CONFIG_HPP__
#define __CONFIG_HPP__

#include <stddef.h> // for size_t

// OS
#ifdef __linux__
	#define MHE_LINUX
#endif

#ifdef WIN32
	#define MHE_WIN
#endif

#ifdef __IOS__ // define before apple
    #define MHE_IOS
#elif defined(__APPLE__) || defined(__MACH__)
	#define MHE_MACOS
#endif

#if !defined(MHE_LINUX) && !defined(MHE_WIN) && !defined(MHE_IOS) && !defined(MHE_MACOS)
	#error Unknown operation system
#endif

// window system
#if defined(MHE_HAS_SDL)
	#define MHE_SDL
#endif

#ifdef MHE_HAS_SDL2
	#define MHE_SDL2
#endif

#ifdef MHE_HAS_QT
	#define MHE_QT
#endif

// video driver
#ifdef MHE_HAS_OPENGL
	#define MHE_OPENGL3
	#define MHE_OPENGL
#endif

#ifdef MHE_HAS_OPENGLES
    #define MHE_OPENGLES
#endif

// audio driver
#ifdef MHE_HAS_OPENAL
	#define MHE_OPENAL
#endif

#ifdef MHE_HAS_VORBIS
	#define MHE_VORBIS
#endif

#ifdef MHE_IOS
#define MHE_STANDART_LOG_DISABLED
#endif

#define LOG_SHADER_INFO
#define RDBG_ENABLED

#if defined RDBG_ENABLED && !defined MHE_NETWORK
#define MHE_NETWORK
#endif

#define PROFILER_ENABLED

#ifdef PROFILER_ENABLED
#define PROFILER_SAVE_LAST_FRAME_DATA
#endif

#define MHE_UPDATE_MATERIAL

#ifdef MHE_HEADERS_ONLY
#define MHE_SYSTEM_NEW_DELETE
#endif

/**
* Rendering configuration
*/
const size_t max_shader_programs_number = 4096;
const size_t max_ubershaders_number = 128;

const size_t initial_material_instances_number = 2048;

const size_t max_trace_data_instances_number = 128;

const size_t max_material_systems_number = 128;
const size_t max_render_targets_number = 128;
const size_t max_simultaneous_render_targets_number = 4;
const size_t max_additional_render_passes_number = 16;

const size_t max_commands_number = 255;

const size_t max_lights_number = 128;

const size_t max_additional_render_passes = 32;

const size_t max_scene_dips_number = 4096;

const size_t default_mips_number = 6;

const size_t shadowmap_default_height = 2048;
const size_t shadowmap_default_width = 2048;

const size_t cubemap_default_texture_size = 128;
const float cubemap_default_size = 10.0f;

const size_t max_shadowmap_cascades_number = 8;
const size_t max_directional_lights_number = 8;

const size_t max_views_number = 16;

const size_t max_managed_render_targets = 16;

const size_t posteffect_material_priority_base = 10;

/**
 * Scene configuration
 */
const size_t max_scene_nodes_number = 4096;

const size_t max_shadowmap_nodes_number = static_cast<size_t>(max_scene_nodes_number * 0.8f);	// 80 %

#endif
