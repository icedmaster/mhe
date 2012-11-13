macro(mhe_add_subproject NAME)
  set(TARGET_NAME ${NAME})
  project(${TARGET_NAME})
  cmake_minimum_required (VERSION 2.8)

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

  include(mhe_platform_specific)
  mhe_link_platform_specific(${TARGET_NAME})
endmacro()
