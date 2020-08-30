add_executable(WaveSabreStandAlonePlayer main.cpp)
target_link_libraries(WaveSabreStandAlonePlayer WaveSabrePlayerLib)

if(MSVC)
	target_compile_definitions(WaveSabreStandAlonePlayer PRIVATE _CRT_SECURE_NO_WARNINGS)

	target_link_libraries(WaveSabreStandAlonePlayer $<$<CONFIG:MinSizeRel>:msvcrt>)
	set_property(TARGET WaveSabreStandAlonePlayer APPEND_STRING PROPERTY LINK_FLAGS_MINSIZEREL
		" /NODEFAULTLIB /SAFESEH:NO /MANIFEST:NO /LTCG /OPT:REF /OPT:ICF /DYNAMICBASE:NO")

	if(MSVC_VERSION GREATER 1900)
		target_compile_definitions(WaveSabreStandAlonePlayer PRIVATE
			$<$<CONFIG:MinSizeRel>:_NO_CRT_STDIO_INLINE>)
	endif()
else()
	# assuming GCC or clang for now

	set(CMAKE_CXX_FLAGS "-fpermissive")
endif()
