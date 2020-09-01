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
	# assuming GCC or clang for now

	if(ENABLE_PTHREADS)
		set_property(TARGET WaveSabrePlayerLib PROPERTY CXX_STANDARD 11) # need a recent one for stdatomic.h

		message(STATUS "Trying to look for pthreads...")
		set(THREADS_PREFER_PTHREAD_FLAG ON)
		find_package(Threads)
		if(CMAKE_USE_PTHREADS_INIT)
			message(STATUS "Found pthreads, enabling pthreads for background rendering threads.")
			#target_link_libraries(WaveSabrePlayerLib PRIVATE Threads::Threads) # we don't actually need it here
			target_compile_definitions(WaveSabrePlayerLib PRIVATE HAVE_PTHREAD=1)
		else()
			message(WARNING "pthreads not found, using synchronous fallback.")
		endif()
	else()
			message(STATUS "pthreads disabled explicitely.")
	endif()

	if(CMAKE_BUILD_TYPE EQUAL Debug)
		target_compile_options(WaveSabreStandAlonePlayer PUBLIC -g -Og)
	else()
		#set_property(TARGET WaveSabreStandAlonePlayer PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)
		target_compile_options(WaveSabreStandAlonePlayer
			PUBLIC -O2 -fno-exceptions -fno-rtti -fno-stack-protector -fno-stack-check -fno-unwind-tables -fno-asynchronous-unwind-tables -fomit-frame-pointer -fno-threadsafe-statics
			PRIVATE -ffast-math -march=nocona -ffunction-sections -fdata-sections -Wl,--gc-sections)
	endif()
endif()
