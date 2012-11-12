# shall be defined
# MHE_DIR
# SOURCE_ROOT
# INCLUDE_ROOT

macro(mhe_based_project NAME)
	set(PROJECT NAME)
	project(${PROJECT})

	set(CMAKE_MODULES_DIR ${MHE_DIR}/build/cmake_modules)
	set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_MODULES_DIR})		

	include(mhe_compiler_defs)
	include(mhe_directories)
	include(find_libraries)

	mhe_setup_directories()
	mhe_setup_compiler_defs()

	file(GLOB HEADERS ${INCLUDE_ROOT}/*.hpp)

	include_directories(${MHE_INCLUDE_DIR})
	include_directories(${MHE_DIR})
	find_library(MHE_LIB mhe ${MHE_LIB_DIR})

	foreach (DIR ${SOURCE_ROOT})
		aux_source_directory (${DIR} TARGET_CUR_SRC)
		set (SOURCES ${SOURCES} ${TARGET_CUR_SRC})
	endforeach ()

	mhe_find_libraries()

	set(LIBS ${LIBS} ${MHE_LIB})

	add_executable(${PROJECT} ${SOURCES})    						  
    target_link_libraries(${PROJECT} ${LIBS})
endmacro()