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

#endif
