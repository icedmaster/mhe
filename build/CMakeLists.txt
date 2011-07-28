
set (PROJECT mhe_test)
project (${PROJECT})

cmake_minimum_required (VERSION 2.8)

set (SOURCE_ROOT ${CMAKE_SOURCE_DIR}/../src)
set (INCLUDE_ROOT ${CMAKE_SOURCE_DIR}/../include)

set (LIB_DIR ${CMAKE_SOURCE_DIR}/../lib)

#set (SRC_DIR)

set (SRC_PATH
  ${SOURCE_ROOT}
  ${SOURCE_ROOT}/gui
  ${SOURCE_ROOT}/impl
  ${SOURCE_ROOT}/utils
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

# find libraries
set(ADD_GL_PATH "/usr/lib/fglrx")
find_library(OPENGL_LIB GL ${ADD_GL_PATH})
message("---- opengl ------")
message(${OPENGL_LIB})

find_library(SDL_LIB SDL)
message("---- sdl ------")
message(${SDL_LIB})

find_library(PNG_LIB png)
message("---- png ------")
message(${PNG_LIB})

find_library(BOOST_DATETIME_LIB boost_date_time)
message("---- boost_date_time ------")
message(${BOOST_DATETIME_LIB})

set (LIBS ${OPENGL_LIB} ${SDL_LIB} ${PNG_LIB} ${BOOST_DATETIME_LIB})

add_executable (${PROJECT} ${SOURCE})
target_link_libraries(${PROJECT} ${LIBS})