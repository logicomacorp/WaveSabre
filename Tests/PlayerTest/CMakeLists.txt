add_executable(PlayerTest main.cpp)
target_link_libraries(PlayerTest WaveSabrePlayerLib)

if(MSVC)
	target_link_libraries(PlayerTest $<$<CONFIG:MinSizeRel>:msvcrt>)
	set_property(TARGET PlayerTest APPEND_STRING PROPERTY LINK_FLAGS_MINSIZEREL
		" /NODEFAULTLIB /SAFESEH:NO /MANIFEST:NO /LTCG /OPT:REF /OPT:ICF /DYNAMICBASE:NO")

	if(MSVC_VERSION GREATER 1900)
		target_compile_definitions(PlayerTest PRIVATE
			$<$<CONFIG:MinSizeRel>:_NO_CRT_STDIO_INLINE>)
	endif()
endif()
