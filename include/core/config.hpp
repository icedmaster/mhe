#ifndef __CONFIG_HPP__
#define __CONFIG_HPP__

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

/**
* Rendering configuration
*/
const size_t max_shader_programs_number = 4096;
const size_t max_ubershaders_number = 128;

const size_t max_material_systems_number = 16;
const size_t max_render_targets_number = 16;
const size_t max_simultaneous_render_targets_number = 4;
const size_t max_additional_render_passes_number = 16;

const size_t max_commands_number = 255;

const size_t max_lights_number = 16;

const size_t max_additional_render_passes = 16;

#endif
