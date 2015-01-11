#output all libs in MHE_LIBS_FOUND

macro(mhe_find_libraries)

  if (NOT DONT_FIND_LIBS)

  if (MSVC)
  	 include_directories(${LIB_DIR}/vs)	 
	 set(LIB_HINT ${LIB_DIR}/vs ${MSVC_HINT_DIR})						 
	 message(${LIB_HINT})
  endif()		

	#find OpenGL
	if (NOT DEFINED IOS)
	  if (CMAKE_HOST_UNIX)
		if (APPLE)
	   	  set(ADD_GL_PATH "/System/Library/Frameworks/OpenGL.framework/Libraries")
		endif()
		find_library(OPENGL_LIB GL ${ADD_GL_PATH})
	  else (CMAKE_HOST_UNIX)
		if (MSVC)
			find_library(OPENGL_LIB OpenGL32 HINTS ${LIB_HINT})
		else ()
	    	find_library(OPENGL_LIB opengl)
		endif()
	  endif()
	  if (${OPENGL_LIB} STREQUAL "OPENGL_LIB-NOTFOUND")
		message("OpenGL lib not found")
	  else()
		message("---- opengl ------")
		message(${OPENGL_LIB})
		add_definitions(-DMHE_HAS_OPENGL)
		set(MHE_OPENGL_FOUND TRUE)
		set(MHE_LIBS_FOUND ${MHE_LIBS_FOUND} ${OPENGL_LIB})
	  endif()
	else() # ios
	  find_library(OPENGLES_LIB OpenGLES)
	  if (${OPENGLES_LIB} STREQUAL "OPENGLES_LIB-NOTFOUND")
		message("OpenGLES lib not found")
	  else()
		message("---- opengles ----")
		message(${OPENGLES_LIB})
		add_definitions(-DMHE_HAS_OPENGLES)
		set(MHE_OPENGLES_FOUND TRUE)
		set(MHE_LIBS_FOUND ${MHE_LIBS_FOUND} ${OPENGLES_LIB})
	  endif()
	endif()

	# find SDL
	# try to find SDL2 then SDL
	# unix check is temporary
	find_library(SDL2_LIB SDL2 HINTS ${LIB_HINT})
	if (${SDL2_LIB} STREQUAL "SDL2_LIB-NOTFOUND")
	    message("SDL2 not found, try to find SDL")
		find_library(SDL_LIB SDL HINTS ${LIB_HINT})
	else()
		set(MHE_SDL2_FOUND TRUE)
		set(SDL_LIB ${SDL2_LIB})
	endif()
	if (${SDL_LIB} STREQUAL "SDL_LIB-NOTFOUND")
	  message("SDL not found")
	else()
	  if (WIN32)
		if (MHE_SDL2_FOUND)
			set(SDLMAIN_LIB_NAME SDL2main)
		else()
			set(SDLMAIN_LIB_NAME SDLmain)
		endif()
		find_library(SDLMAIN_LIB ${SDLMAIN_LIB_NAME} HINTS ${LIB_HINT})
		if (NOT MSVC)
	   		set(SDL_LIB mingw32 ${SDLMAIN_LIB} ${SDL_LIB})
		else()
			set(SDL_LIB ${SDLMAIN_LIB} ${SDL_LIB})
		endif()
	  endif()
	  if (APPLE)
		find_library(SDLMAIN_LIB SDLmain)
		set(SDL_LIB ${SDLMAIN_LIB} ${SDL_LIB})
	  endif()
	  message("---- sdl ------")
	  message(${SDL_LIB})
	  if (NOT MHE_SDL2_FOUND)	
		  add_definitions(-DMHE_HAS_SDL)
	  else()
		  add_definitions(-DMHE_HAS_SDL2)
	  endif()
	  set(MHE_SDL_FOUND TRUE)
	  set(MHE_LIBS_FOUND ${MHE_LIBS_FOUND} ${SDL_LIB})
	endif()

	# find Qt
	set(QT_USE_QTOPENGL TRUE)
	set(DESIRED_QT_VERSION 4)
	find_package(Qt)
	if (QT_FOUND)
	  message("Find Qt libraries")
	  add_definitions(-DMHE_HAS_QT)
	  set(MHE_QT_FOUND TRUE)
	  include(${QT_USE_FILE})
	  add_definitions(${QT_DEFINITIONS})
	  file(GLOB MHE_QT_HEADERS ${PLATFORM_IMPL_HEADERS}/qt/*.hpp)
	  qt4_wrap_cpp(QT_HEADERS_MOC ${MHE_QT_HEADERS})
	  set(ADDITIONAL_SOURCES ${ADDITIONAL_SOURCES} ${QT_HEADERS_MOC})
	  set(MHE_LIBS_FOUND ${MHE_LIBS_FOUND} ${QT_LIBRARIES})
	else()
	  message("Qt libs not found")
	endif()

	# find libpng
	if (NOT DEFINED IOS)
	  set(PNG_LIB_NAME png)
	  if (MSVC)
	      set(PNG_LIB_NAME libpng)
	  endif()
	  find_library(PNG_LIB ${PNG_LIB_NAME} HINTS ${LIB_HINT})
	else()
	  set(ADD_PNG_PATH ${IOS_LIB_DIR}/debug)
	  set(PNG_LIB ${ADD_PNG_PATH}/libpng.a)
	endif()  
	message("---- png ------")
	message(${PNG_LIB})
	set(MHE_LIBS_FOUND ${MHE_LIBS_FOUND} ${PNG_LIB})		

	# find OpenAL
	if (WIN32)
	  find_library(OPENAL_LIB openal32)
	else()					
	  find_library(OPENAL_LIB openal)
	endif()
	if (${OPENAL_LIB} STREQUAL "OPENAL_LIB-NOTFOUND")
      message("OpenAL not found")
	else()
	  message("---- OpenAL -----")
	  message(${OPENAL_LIB})
	  add_definitions(-DMHE_HAS_OPENAL)
	  set(MHE_OPENAL_FOUND TRUE)
	  set(MHE_LIBS_FOUND ${MHE_LIBS_FOUND} ${OPENAL_LIB})
	endif()	

	find_library(VORBISFILE_LIB vorbisfile)
	if (${VORBISFILE_LIB} STREQUAL "VORBISFILE_LIB-NOTFOUND")
	  message("Vorbisfile lib not found")
	else()
	  message("---- vorbisfile ----")
	  message(${VORBISFILE_LIB})

	  find_library(VORBIS_LIB vorbis)
	  if (${VORBIS_LIB} STREQUAL "VORBIS_LIB-NOTFOUND")
		message("Vorbislib not found")
	  else()
		message("---- vorbis ----")
		message(${VORBIS_LIB})

		find_library(OGG_LIB ogg)
		if (${OGG_LIB} STREQUAL "OGG_LIB-NOTFOUND")
		  message("ogglib not found")
		else()
		  message("---- ogg ----")
		  message(${OGG_LIB})
		  set(MHE_LIBS_FOUND ${MHE_LIBS_FOUND} ${VORBISFILE_LIB} ${VORBIS_LIB} ${OGG_LIB})
		  add_definitions(-DMHE_HAS_VORBIS)
		  set(MHE_VORBIS_FOUND TRUE)
		endif()
	  endif()
	endif()

	if (APPLE AND NOT DEFINED IOS)
	  set(MHE_LIBS_FOUND ${MHE_LIBS_FOUND} dl)
	elseif (WIN32)
	  message("WIN32")
	  set(MHE_LIBS_FOUND ${MHE_LIBS_FOUND} Winmm)
	else ()
	  	set(MHE_LIBS_FOUND ${MHE_LIBS_FOUND} dl rt)
	endif()

	if (USE_ASSIMP_LIB)
	  find_library(ASSIMP_LIB assimp HINTS ${LIB_HINT})
	  if (${ASSIMP_LIB} STREQUAL "ASSIMP_LIB-NOTFOUND")
		message("Assimp lib is not found")
	  else()
		message("---------- assimp ------------")
		message(${ASSIMP_LIB})
		set(MHE_LIBS_FOUND ${MHE_LIBS_FOUND} ${ASSIMP_LIB})
	  endif()
	endif()

  endif() #DONT_FIND_LIBRARIES

endmacro()