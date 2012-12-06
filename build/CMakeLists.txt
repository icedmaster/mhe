set (PROJECT mhe)
project (${PROJECT})

cmake_minimum_required (VERSION 2.8)

# add cmake modules
set(CMAKE_MODULES_DIR ${CMAKE_SOURCE_DIR}/cmake_modules)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_MODULES_DIR})

# init build options
option(BUILD_EXAMPLES "Build mhe examples" OFF)
option(BUILD_LIB_ONLY "Build only libmhe, without examples and demo" ON)
option(BUILD_WITH_SCRIPTS "Build with script languages support" ON)
option(BUILD_IOS "Build for iOS" OFF)

include(add_subproject)

# defaults for build
if (BUILD_EXAMPLES)
   message("Build examples option is ON")
   set(MHE_BUILD_EXAMPLES TRUE)
   set(BUILD_LIB_ONLY FALSE)
endif()

if (BUILD_LIB_ONLY)
    message("Build mhelib only")
    set(MHE_BUILD_EXAMPLES FALSE)
endif()

if (BUILD_OS)
  set(IO TRUE)
endif()

if (IOS)
   message("Build for iOS")
   include(mhe_ios)
   mhe_ios_build() 
endif()

add_definitions(-Wall)
add_definitions(-Wextra)

if (CMAKE_BUILD_TYPE STREQUAL "Release")
   add_definitions(-O2)
endif()

set (SOURCE_ROOT ${CMAKE_SOURCE_DIR}/../src)
set (INCLUDE_ROOT ${CMAKE_SOURCE_DIR}/../include)

set (LIB_DIR ${CMAKE_SOURCE_DIR}/../lib)

set(IOS_LIB_DIR ${CMAKE_SOURCE_DIR}/../lib/ios)

#set (SRC_DIR)

set (SRC_PATH
  ${SOURCE_ROOT}
  ${SOURCE_ROOT}/gui
  ${SOURCE_ROOT}/impl
  ${SOURCE_ROOT}/utils
  ${SOURCE_ROOT}/sound
  ${SOURCE_ROOT}/game
  ${SOURCE_ROOT}/events
  ${SOURCE_ROOT}/app
  #${SOURCE_ROOT}/script
  ${LIB_DIR}/pugixml
)

set (STANDALONE_SOURCES "")

set (INC_PATH
  ${INCLUDE_ROOT}
  ${INCLUDE_ROOT}/..
)

set(PLATFORM_IMPL_SOURCES ${SOURCE_ROOT}/platform)
set(OPENGL_IMPL_SOURCES ${PLATFORM_IMPL_SOURCES}/opengl)
set(OPENGLES_IMPL_SOURCES ${PLATFORM_IMPL_SOURCES}/opengles)
set(LINUX_PLATFORM_SOURCES ${PLATFORM_IMPL_SOURCES}/linux)
set(WIN_PLATFORM_SOURCES ${PLATFORM_IMPL_SOURCES}/win)
set(MACOS_PLATFORM_SOURCES ${PLATFORM_IMPL_SOURCES}/macos)
set(IOS_PLATFORM_SOURCES ${PLATFORM_IMPL_SOURCES}/ios)

set(VORBIS_SOURCES ${SOURCE_ROOT}/sound/vorbis)

# platform
if (WIN32)
   add_definitions(-DMHE_OS_WIN)
   set(SRC_PATH ${WIN_PLATFORM_SOURCES} ${SRC_PATH})
elseif (NOT APPLE)
add_definitions(-DMHE_OS_LINUX)
set(SRC_PATH ${LINUX_PLATFORM_SOURCES} ${SRC_PATH})
endif()

if (IOS)
   add_definitions(-DMHE_OS_IOS)
   include_directories("/opt/local/include")
   set(SRC_PATH ${IOS_PLATFORM_SOURCES} ${SRC_PATH})
elseif (APPLE)
    add_definitions(-DMHE_OS_MAC)
    include_directories("/opt/local/include")
    include_directories("/usr/X11R6/include")
    include_directories("/System/Library/Frameworks/OpenAL.framework/Headers")
    set(SRC_PATH ${MACOS_PLATFORM_SOURCES} ${SRC_PATH})
endif()


include_directories (${INC_PATH})

include(find_libraries)

mhe_find_libraries()

# find libraries
if (MHE_OPENGL_FOUND)
   # add platform sources
   set(SRC_PATH ${OPENGL_IMPL_SOURCES} ${SRC_PATH})
   set(LIBS ${LIBS} ${OPENGL_LIB})
endif()

if (MHE_OPENGLES_FOUND)
   set(SRC_PATH ${OPENGLES_IMPL_SOURCES} ${SRC_PATH})
   set(LIBS ${LIBS} ${OPENGLES_LIB})
   set(STANDALONE_SOURCES ${STANDALONE_SOURCES} ${OPENGL_IMPL_SOURCES}/opengl_texture.cpp
	 ${OPENGL_IMPL_SOURCES}/opengl_extension.cpp)
endif()

if (MHE_SDL_FOUND)
   set(SRC_PATH ${SRC_PATH} ${PLATFORM_IMPL_SOURCES}/sdl)
   set(LIBS ${LIBS} ${SDL_LIB})
endif()

if (MHE_VORBIS_FOUND)
  set(SRC_PATH ${SRC_PATH} ${VORBIS_SOURCES})
endif()

set (LIBS ${LIBS} ${PNG_LIB})

if (NOT DEFINED IOS)
  set(LIBS ${LIBS} ${OPENAL_LIB}
	${VORBISFILE_LIB} ${VORBIS_LIB} ${OGG_LIB})
  if (WIN32)
	set (LIBS ${LIBS} Ws2_32)
  endif()
endif()

# find scripts
if (BUILD_WITH_SCRIPTS)
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
endif()

foreach (DIR ${SRC_PATH})
  aux_source_directory (${DIR} CUR_SRC)
  set (SOURCE ${SOURCE} ${CUR_SRC})
endforeach ()

set(SOURCE ${SOURCE} ${STANDALONE_SOURCES})

# additional libraries
if (WIN32)
	set(LIBS ${LIBS} winmm)
endif()
if (APPLE AND NOT __IOS__)
    set(LIBS ${LIBS} dl)
else()
    set(LIBS ${LIBS} dl rt)
endif()

set(MHE_LIB_DIR ${CMAKE_SOURCE_DIR})
set(DEP_LIBS ${LIBS})

add_library(${PROJECT} ${SOURCE})
target_link_libraries(${PROJECT} ${LIBS})

include(mhe_platform_specific)
mhe_link_platform_specific(${PROJECT})

if (MHE_BUILD_DEMO)
	message("Build demo")
	add_subdirectory(${CMAKE_SOURCE_DIR}/../demo/build/ ${CMAKE_SOURCE_DIR}/../demo/build/)
endif()

if (MHE_BUILD_EXAMPLES)
	message("Build examples")
	add_subdirectory(${CMAKE_SOURCE_DIR}/../examples/build/ ${CMAKE_SOURCE_DIR}/../examples/build/)
endif()

