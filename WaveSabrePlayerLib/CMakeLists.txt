add_library(WaveSabrePlayerLib
	include/WaveSabrePlayerLib/CriticalSection.h
	include/WaveSabrePlayerLib/IRenderThread.h
	include/WaveSabrePlayerLib/PreRenderPlayer.h
	include/WaveSabrePlayerLib/WavWriter.h
	include/WaveSabrePlayerLib/IPlayer.h
	include/WaveSabrePlayerLib/RealtimePlayer.h
	include/WaveSabrePlayerLib/SongRenderer.h
	src/CriticalSection.cpp
	src/IPlayer.cpp
	src/IRenderThread.cpp
	src/PreRenderPlayer.cpp
	src/RealtimePlayer.cpp
	src/SongRenderer.cpp
	src/SongRenderer.Track.cpp
	src/WavWriter.cpp)

target_link_libraries(WaveSabrePlayerLib PUBLIC
	WaveSabreCore)

if(WIN32)
	message(STATUS "Win32: using DirectSound renderthread")
	target_sources(WaveSabrePlayerLib PRIVATE
		include/WaveSabrePlayerLib/DirectSoundRenderThread.h
		src/DirectSoundRenderThread.cpp)

	target_link_libraries(WaveSabrePlayerLib PUBLIC
		winmm.lib
		dsound.lib)
else()
	if(ENABLE_APLAY)
		message(STATUS "Enabling aplay(1)-based backend")
		target_compile_definitions(WaveSabrePlayerLib PRIVATE HAVE_APLAY=1)
		target_sources(WaveSabrePlayerLib PRIVATE
			include/WaveSabrePlayerLib/AplayRenderThread.h
			src/AplayRenderThread.cpp)
	else()
		message(STATUS "aplay(1) backend disabled")
	endif()

	if(ENABLE_SDL2)
		message(STATUS "Enabling SDL2 backend")

		find_package(SDL2)
		if(SDL2_FOUND)
			message(STATUS "Found SDL2, enabling SDL2 backend")
			target_link_libraries(WaveSabrePlayerLib PUBLIC SDL2::SDL2 SDL2::SDL2main)
			target_compile_definitions(WaveSabrePlayerLib PUBLIC HAVE_SDL2=1)
			target_sources(WaveSabrePlayerLib PRIVATE
				include/WaveSabrePlayerLib/SDL2RenderThread.h
				src/SDL2RenderThread.cpp)
		else()
			message(WARNING "SDL2 not found, disabling SDL2 backend")
		endif()
	else()
		message(STATUS "SDL2 backend disabled")
	endif()

	if(USE_PTHREADS)
		set_property(TARGET WaveSabrePlayerLib PROPERTY CXX_STANDARD 11) # need a recent one for stdatomic.h

		message(STATUS "Trying to look for pthreads...")
		set(THREADS_PREFER_PTHREAD_FLAG ON)
		find_package(Threads)
		if(CMAKE_USE_PTHREADS_INIT)
			message(STATUS "Found pthreads, enabling pthreads for background rendering threads.")
			target_link_libraries(WaveSabrePlayerLib PRIVATE Threads::Threads)
			target_compile_definitions(WaveSabrePlayerLib PRIVATE HAVE_PTHREAD=1)
		else()
			message(WARNING "pthreads not found, using synchronous fallback.")
		endif()
	else()
			message(STATUS "pthreads disabled explicitely.")
	endif()
endif()

target_include_directories(WaveSabrePlayerLib PUBLIC include)

if(MSVC)
	target_compile_definitions(WaveSabrePlayerLib PRIVATE _CRT_SECURE_NO_WARNINGS)
	target_compile_options(WaveSabrePlayerLib PUBLIC
		$<$<CONFIG:MinSizeRel>:/EHs-c->)
	set_property(TARGET WaveSabrePlayerLib APPEND_STRING PROPERTY STATIC_LIBRARY_FLAGS_MINSIZEREL
		" /LTCG")
else()
	# assuming GCC or clang for now

	if(CMAKE_BUILD_TYPE EQUAL Debug)
		set(CMAKE_CXX_FLAGS "-g -Og")
	else()
		set_property(TARGET  PROPERTY INTERPROCEDURAL_OPTIMIZATION True) # enable LTO
		set(CMAKE_CXX_FLAGS "-g -O2 -ffast-math -fno-exceptions -fno-rtti -fno-stack-protector -fno-stack-check -fno-unwind-tables -fno-asynchronous-unwind-tables -fomit-frame-pointer -fno-threadsafe-statics -march=nocona -ffunction-sections -fdata-sections -Wl,--gc-sections")
	endif()
	if(CMAKE_BUILD_TYPE EQUAL Debug)
		target_compile_options(WaveSabrePlayerLib PUBLIC -g -Og)
	else()
		#set_property(TARGET WaveSabrePlayerLib PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)
		target_compile_options(WaveSabrePlayerLib
			PUBLIC -O2 -fno-exceptions -fno-rtti -fno-stack-protector -fno-stack-check -fno-unwind-tables -fno-asynchronous-unwind-tables -fomit-frame-pointer -fno-threadsafe-statics
			PRIVATE -ffast-math -march=nocona -ffunction-sections -fdata-sections -Wl,--gc-sections)
	endif()
endif()
