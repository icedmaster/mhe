#ifndef __CONFIG_HPP__
#define __CONFIG_HPP__

// OS
#ifdef __linux__
	#define MHE_LINUX
#elif defined(__WIN32__)
	#define MHE_WIN
#elif defined(__APPLE__) || defined(__MACH__)
	#define MHE_MACOS
#else
	#error Unknown operation system
#endif

// window system
#ifdef MHE_HAS_SDL
	#define MHE_SDL
#endif

#ifdef MHE_HAS_QT
	#define MHE_QT
#endif

// video driver
#ifdef MHE_HAS_OPENGL
	#define MHE_OPENGL
#endif

// audio driver
#ifdef MHE_HAS_OPENAL
	#define MHE_OPENAL
#endif

#endif
