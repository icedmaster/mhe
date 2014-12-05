macro(mhe_setup_compiler_defs)

	if (NOT MSVC)
	  add_definitions(-Wall)
	  add_definitions(-Wextra)
	endif()
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

	if (MSVC)
		set(MSVC_HINT_DIR "C:/Program Files (x86)/Microsoft SDKs/Windows/v7.0A/Lib" "C:/Program Files (x86)/Microsoft SDKs/Windows/v7.1A/Lib")
	endif()

endmacro()
