add_library(WaveSabrePlayerLib
	include/WaveSabrePlayerLib/IRenderThread.h
	include/WaveSabrePlayerLib/PreRenderPlayer.h
	include/WaveSabrePlayerLib/WavWriter.h
	include/WaveSabrePlayerLib/IPlayer.h
	include/WaveSabrePlayerLib/RealtimePlayer.h
	include/WaveSabrePlayerLib/SongRenderer.h
	src/IPlayer.cpp
	src/IRenderThread.cpp
	src/PreRenderPlayer.cpp
	src/RealtimePlayer.cpp
	src/SongRenderer.cpp
	src/SongRenderer.Track.cpp
	src/WavWriter.cpp)

target_link_libraries(WaveSabrePlayerLib
	WaveSabreCore)

if(WIN32)
	add_library(WaveSabrePlayerLib
		include/WaveSabrePlayerLib/CriticalSection.h
		include/WaveSabrePlayerLib/DirectSoundRenderThread.h
		src/CriticalSection.cpp
		src/DirectSoundRenderThread.cpp)

	target_link_libraries(WaveSabrePlayerLib
		winmm.lib
		dsound.lib)
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

	set_property(TARGET WaveSabrePlayerLib PROPERTY INTERPROCEDURAL_OPTIMIZATION True) # enable LTO
	# -fpermissive + stuff to make the code smaller
	set(CMAKE_CXX_FLAGS "-fpermissive -g -O2 -ffast-math -fno-exceptions -fno-rtti -fno-stack-protector -fno-stack-check -fno-unwind-tables -fno-asynchronous-unwind-tables -fomit-frame-pointer -fno-threadsafe-statics -march=nocona -ffunction-sections -fdata-sections -Wl,--gc-sections")
endif()
