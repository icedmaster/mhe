
set (PROJECT mhe_test)
project (${PROJECT})

cmake_minimum_required (VERSION 2.8)

add_definitions(-Wall)
add_definitions(-Wextra)

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
  ${LIB_DIR}/pugixml
)

set (INC_PATH
  ${INCLUDE_ROOT}
  ${INCLUDE_ROOT}/..
)

include_directories (${INC_PATH})

foreach (DIR ${SRC_PATH})
  aux_source_directory (${DIR} CUR_SRC)
  set (SOURCE ${SOURCE} ${CUR_SRC})
endforeach ()

set (SOURCE ${SOURCE} ${CMAKE_SOURCE_DIR}/../main.cpp)
#set (SOURCE ${SOURCE} ${CMAKE_SOURCE_DIR}/../test_main.cpp)

# find libraries
if (CMAKE_HOST_UNIX)
   set(ADD_GL_PATH "/usr/lib/fglrx")
   find_library(OPENGL_LIB GL ${ADD_GL_PATH})
else (CMAKE_HOST_UNIX)
   find_library(OPENGL_LIB opengl32)
endif()
message("---- opengl ------")
message(${OPENGL_LIB})

find_library(SDL_LIB SDL)
if (WIN32)
   find_library(SDLMAIN_LIB SDLmain)		
   set(SDL_LIB mingw32 ${SDLMAIN_LIB} ${SDL_LIB})
endif()
message("---- sdl ------")
message(${SDL_LIB})

find_library(PNG_LIB png)
message("---- png ------")
message(${PNG_LIB})

find_library(BOOST_DATETIME_LIB boost_date_time)
message("---- boost_date_time ------")
message(${BOOST_DATETIME_LIB})

if (WIN32)
   find_library(OPENAL_LIB openal32)
else()
find_library(OPENAL_LIB openal)
endif()
message("---- OpenAL -----")
message(${OPENAL_LIB})

find_library(VORBISFILE_LIB vorbisfile)
message("---- vorbisfile ----")
message(${VORBISFILE_LIB})

find_library(VORBIS_LIB vorbis)
message("---- vorbis ----")
message(${VORBIS_LIB})

find_library(OGG_LIB ogg)
message("---- ogg ----")
message(${OGG_LIB})

find_library(GTEST_LIB gtest)
message("---- gtest ----")
message(${GTEST_LIB})

set (LIBS ${OPENGL_LIB} ${SDL_LIB} ${PNG_LIB} ${BOOST_DATETIME_LIB} ${OPENAL_LIB}
	${VORBISFILE_LIB} ${VORBIS_LIB} ${OGG_LIB}
	${GTEST_LIB})
if (WIN32)
   set (LIBS ${LIBS} Ws2_32)
endif()

add_executable (${PROJECT} ${SOURCE})
target_link_libraries(${PROJECT} ${LIBS})

if (WIN32)
   set_target_properties(${PROJECT} PROPERTIES LINK_FLAGS "-Wl,--enable-stdcall-fixup")
endif()
