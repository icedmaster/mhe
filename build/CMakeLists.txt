set (PROJECT mhe)
project (${PROJECT})

cmake_minimum_required (VERSION 2.8)

# defaults for build
set(MHE_BUILD_DEMO TRUE)

if (DEFINED LIB_ONLY)
    message("Build mhelib only")
    set(MHE_BUILD_DEMO FALSE)
endif()

add_definitions(-Wall)
add_definitions(-Wextra)

if (CMAKE_BUILD_TYPE STREQUAL "Release")
	add_definitions(-O2)
endif()

set (SOURCE_ROOT ${CMAKE_SOURCE_DIR}/../src)
set (INCLUDE_ROOT ${CMAKE_SOURCE_DIR}/../include)

set (LIB_DIR ${CMAKE_SOURCE_DIR}/../lib)

#set (SRC_DIR)

set (SRC_PATH
  ${SOURCE_ROOT}
  ${SOURCE_ROOT}/gui
  ${SOURCE_ROOT}/impl
  ${SOURCE_ROOT}/utils
  ${SOURCE_ROOT}/sound
  ${SOURCE_ROOT}/game
  #${SOURCE_ROOT}/script
  ${LIB_DIR}/pugixml
)

set (INC_PATH
  ${INCLUDE_ROOT}
  ${INCLUDE_ROOT}/..
)

set(PLATFORM_IMPL_SOURCES ${SOURCE_ROOT}/platform)
set(OPENGL_IMPL_SOURCES ${PLATFORM_IMPL_SOURCES}/opengl)
set(LINUX_PLATFORM_SOURCES ${PLATFORM_IMPL_SOURCES}/linux)

# platform
if (WIN32)
   add_definitions(-DMHE_WIN32)
else()
	add_definitions(-DMHE_LINUX)		
	set(SRC_PATH ${LINUX_PLATFORM_SOURCES} ${SRC_PATH})
endif()

include_directories (${INC_PATH})

# find libraries
if (CMAKE_HOST_UNIX)
   set(ADD_GL_PATH "/usr/lib/fglrx")
   find_library(OPENGL_LIB GL ${ADD_GL_PATH})
else (CMAKE_HOST_UNIX)
   find_library(OPENGL_LIB opengl32)
endif()
if (${OPENGL_LIB} STREQUAL "OPENGL_LIB-NOTFOUND")
	message("OpenGL lib not found")
else()
	message("---- opengl ------")
	message(${OPENGL_LIB})
	add_definitions(-DMHE_OPENGL)
	# add platform sources
	set(SRC_PATH ${OPENGL_IMPL_SOURCES} ${SRC_PATH})
endif()

find_library(SDL_LIB SDL)
if (${SDL_LIB} STREQUAL "SDL_LIB-NOTFOUND")
	message("SDL not found")
else()
	if (WIN32)
	    find_library(SDLMAIN_LIB SDLmain)		
   		set(SDL_LIB mingw32 ${SDLMAIN_LIB} ${SDL_LIB})
	endif()
	message("---- sdl ------")
	message(${SDL_LIB})
	add_definitions(-DMHE_SDL)
	set(SRC_PATH ${SRC_PATH} ${PLATFORM_IMPL_SOURCES}/sdl)
endif()	

find_library(PNG_LIB png)
message("---- png ------")
message(${PNG_LIB})

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
  add_definitions(-DMHE_OPENAL)
endif()

find_library(VORBISFILE_LIB vorbisfile)
message("---- vorbisfile ----")
message(${VORBISFILE_LIB})

find_library(VORBIS_LIB vorbis)
message("---- vorbis ----")
message(${VORBIS_LIB})

find_library(OGG_LIB ogg)
message("---- ogg ----")
message(${OGG_LIB})

set (LIBS ${OPENGL_LIB} ${SDL_LIB} ${PNG_LIB} ${BOOST_DATETIME_LIB} ${OPENAL_LIB}
	${VORBISFILE_LIB} ${VORBIS_LIB} ${OGG_LIB})
if (WIN32)
   set (LIBS ${LIBS} Ws2_32)
endif()

if (MHE_BUILD STREQUAL "test")
	find_library(GTEST_LIB gtest)
	message("---- gtest ----")
	message(${GTEST_LIB})
	set (LIBS ${LIBS} ${GTEST_LIB})
endif()

# find scripts
find_library(PYTHON_LIB python3)
message("---- python ----")
if (${PYTHON_LIB} STREQUAL "PYTHON_LIB-NOTFOUND")
  message("Python lib not found. Python scripting not availible")
else()
  message(${PYTHON_LIB})
  message("---- boost_python ----")
  find_library(BOOST_PYTHON_LIB boost_python3)
  message(${BOOST_PYTHON_LIB})
  set(LIBS ${LIBS} ${BOOST_PYTHON_LIB} ${PYTHON_LIB})
endif()

foreach (DIR ${SRC_PATH})
  aux_source_directory (${DIR} CUR_SRC)
  set (SOURCE ${SOURCE} ${CUR_SRC})
endforeach ()

if (MHE_BUILD STREQUAL "test")
	message("Create test target makefile")
	set (SOURCE ${SOURCE} ${CMAKE_SOURCE_DIR}/../test_main.cpp)
endif()

set(DEP_LIBS ${LIBS})

if (MHE_BUILD_DEMO)
	message("Build demo")
	add_subdirectory(${CMAKE_SOURCE_DIR}/../demo/build/ ${CMAKE_SOURCE_DIR}/../demo/build/)
endif()

add_library(${PROJECT} ${SOURCE})
target_link_libraries(${PROJECT} ${LIBS})

if (WIN32)
   set_target_properties(${PROJECT} PROPERTIES LINK_FLAGS "-Wl,--enable-stdcall-fixup")
endif()
