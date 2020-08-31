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

	target_link_libraries(WaveSabrePlayerLib
		winmm.lib
		dsound.lib)
else()
	if(ENABLE_APLAY)
		message(STATUS "Enabling aplay(1)-based backend")
		target_compile_definitions(WaveSabrePlayerLib PRIVATE HAVE_APLAY=1)
		target_sources(WaveSabrePlayerLib PRIVATE
			include/WaveSabrePlayerLib/AplayRenderThread.h
			src/AplayRenderThread.cpp)
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
			message(STATUS "pthreads not found, using synchronous fallback.")
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

	set(CMAKE_CXX_FLAGS "-g -Og")
	# stuff to make the code smaller
	#set_property(TARGET WaveSabrePlayerLib PROPERTY INTERPROCEDURAL_OPTIMIZATION True) # enable LTO
	#set(CMAKE_CXX_FLAGS "-g -O2 -ffast-math -fno-exceptions -fno-rtti -fno-stack-protector -fno-stack-check -fno-unwind-tables -fno-asynchronous-unwind-tables -fomit-frame-pointer -fno-threadsafe-statics -march=nocona -ffunction-sections -fdata-sections -Wl,--gc-sections")
endif()
