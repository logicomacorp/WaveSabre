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

	set(CMAKE_CXX_FLAGS "-g -Og")
	# stuff to make the code smaller
	#set_property(TARGET WaveSabrePlayerLib PROPERTY INTERPROCEDURAL_OPTIMIZATION True) # enable LTO
	#set(CMAKE_CXX_FLAGS "-g -O2 -ffast-math -fno-exceptions -fno-rtti -fno-stack-protector -fno-stack-check -fno-unwind-tables -fno-asynchronous-unwind-tables -fomit-frame-pointer -fno-threadsafe-statics -march=nocona -ffunction-sections -fdata-sections -Wl,--gc-sections")
endif()
