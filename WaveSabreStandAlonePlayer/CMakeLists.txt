add_executable(WaveSabreStandAlonePlayer main.cpp)
target_link_libraries(WaveSabreStandAlonePlayer PUBLIC WaveSabrePlayerLib)

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
	#if(ENABLE_SDL2)
	#	message(STATUS "Enabling SDL2 backend")
	#
	#	find_package(SDL2)
	#	if(SDL2_FOUND)
	#		message(STATUS "Found SDL2, enabling SDL2 backend")
	#		target_link_libraries(WaveSabreStandAlonePlayer PUBLIC SDL2::SDL2 SDL2::SDL2main)
	#		target_compile_definitions(WaveSabreStandAlonePlayer PUBLIC HAVE_SDL2=1)
	#	else()
	#		message(WARNING "SDL2 not found, disabling SDL2 backend")
	#	endif()
	#else()
	#	message(STATUS "SDL2 backend disabled")
	#endif()

	# assuming GCC or clang for now

	if(CMAKE_BUILD_TYPE EQUAL Debug)
		target_compile_options(WaveSabreStandAlonePlayer PUBLIC -g -Og)
	else()
		#set_property(TARGET WaveSabreStandAlonePlayer PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)
		target_compile_options(WaveSabreStandAlonePlayer
			PUBLIC -O2 -fno-exceptions -fno-rtti -fno-stack-protector -fno-stack-check -fno-unwind-tables -fno-asynchronous-unwind-tables -fomit-frame-pointer -fno-threadsafe-statics
			PRIVATE -ffast-math -march=nocona -ffunction-sections -fdata-sections -Wl,--gc-sections)
	endif()
endif()
