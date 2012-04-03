set (DEMO_PROJECT mhe_demo)
project (${DEMO_PROJECT})

cmake_minimum_required (VERSION 2.8)

add_definitions(-Wall)
add_definitions(-Wextra)

if (CMAKE_BUILD_TYPE STREQUAL "Release")
	add_definitions(-O2)
endif()

set (DEMO_SOURCE_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/..)
set (DEMO_INCLUDE_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../../include)

set (DEMO_SRC_PATH
  ${DEMO_SOURCE_ROOT}
)

message(${DEMO_SRC_PATH})

set (INC_PATH
  ${DEMO_INCLUDE_ROOT}
)

include_directories (${INC_PATH})

foreach (DIR ${DEMO_SRC_PATH})
  aux_source_directory (${DIR} DEMO_CUR_SRC)
  set (DEMO_SOURCE ${DEMO_SOURCE} ${DEMO_CUR_SRC})
endforeach ()

# add mhe lib
set(MHE_LIB_DIR ${CMAKE_SOURCE_DIR})
find_library(MHE_LIB mhe ${MHE_LIB_DIR})

set(DEMO_LIBS ${MHE_LIB} ${DEP_LIBS})

foreach (S ${DEMO_LIBS})
	message(${S})
endforeach()

add_executable(${DEMO_PROJECT} ${DEMO_SOURCE})
target_link_libraries(${DEMO_PROJECT} ${DEMO_LIBS})

if (WIN32)
   set_target_properties(${DEMO_PROJECT} PROPERTIES LINK_FLAGS "-Wl,--enable-stdcall-fixup")
endif()
