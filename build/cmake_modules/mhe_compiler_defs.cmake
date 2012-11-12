macro(mhe_setup_compiler_defs)

	add_definitions(-Wall)
	add_definitions(-Wextra)
	#add_definitions(-std=c++0x)

	if (CMAKE_BUILD_TYPE STREQUAL "Release")
	    add_definitions(-O2)
	else()	
	    add_definitions(-g)	
	endif()

	if (APPLE)
       include_directories("/opt/local/include")
       include_directories("/usr/X11R6/include")
       include_directories("/System/Library/Frameworks/OpenAL.framework/Headers")
	endif()

endmacro()