macro(mhe_ios_build)
	add_definitions(-D__IOS__)
	set(IOS TRUE)
	set(MHE_HAS_OPENGLES TRUE)
endmacro()