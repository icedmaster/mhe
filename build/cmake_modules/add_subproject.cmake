macro(mhe_add_subproject NAME)
  set(TARGET_NAME ${NAME})
  project(${TARGET_NAME})
  cmake_minimum_required (VERSION 2.8)

  # TODO: move to complilers_def.cmake
  add_definitions(-Wall)
  add_definitions(-Wextra)

  if (CMAKE_BUILD_TYPE STREQUAL "Release")
	add_definitions(-O2)
  endif()

  set (TARGET_SOURCE_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/..)
  # main mhe include
  set (TARGET_INCLUDE_ROOT ${INCLUDE_ROOT})

  include_directories (${TARGET_INCLUDE_ROOT})

  foreach (DIR ${TARGET_SOURCE_ROOT})
	aux_source_directory (${DIR} TARGET_CUR_SRC)
	set (TARGET_SOURCE ${TARGET_SOURCE} ${TARGET_CUR_SRC})
  endforeach ()

  # add mhe lib
  link_directories(${MHE_LIB_DIR})
  set(MHE_LIB "-lmhe")

  set(TARGET_LIBS ${MHE_LIB} ${DEP_LIBS})

  add_executable(${TARGET_NAME} ${TARGET_SOURCE})
  target_link_libraries(${TARGET_NAME} ${TARGET_LIBS})

  if (WIN32)
	set_target_properties(${TARGET_NAME} PROPERTIES LINK_FLAGS "-Wl,--enable-stdcall-fixup")
  endif()
endmacro()
