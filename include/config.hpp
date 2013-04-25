#ifndef __CONFIG_HPP__
#define __CONFIG_HPP__

// OS
#ifdef __linux__
	#define MHE_LINUX
#elif defined(__WIN32__)
	#define MHE_WIN
#elif defined(__IOS__) // define before apple
    #define MHE_IOS
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
