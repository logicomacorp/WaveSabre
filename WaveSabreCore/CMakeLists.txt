add_library(WaveSabreCore
	include/WaveSabreCore/Adultery.h
	include/WaveSabreCore/AllPass.h
	include/WaveSabreCore/AllPassDelay.h
	include/WaveSabreCore/BiquadFilter.h
	include/WaveSabreCore/Cathedral.h
	include/WaveSabreCore/Chamber.h
	include/WaveSabreCore/Comb.h
	include/WaveSabreCore/Crusher.h
	include/WaveSabreCore/DelayBuffer.h
	include/WaveSabreCore/Device.h
	include/WaveSabreCore/Echo.h
	include/WaveSabreCore/Envelope.h
	include/WaveSabreCore/Falcon.h
	include/WaveSabreCore/Helpers.h
	include/WaveSabreCore/Leveller.h
	include/WaveSabreCore/MxcsrFlagGuard.h
	include/WaveSabreCore/ResampleBuffer.h
	include/WaveSabreCore/SamplePlayer.h
	include/WaveSabreCore/Scissor.h
	include/WaveSabreCore/Slaughter.h
	include/WaveSabreCore/Smasher.h
	include/WaveSabreCore/Specimen.h
	include/WaveSabreCore/StateVariableFilter.h
	include/WaveSabreCore/SynthDevice.h
	include/WaveSabreCore/Thunder.h
	include/WaveSabreCore/Twister.h
	src/Adultery.cpp
	src/AllPass.cpp
	src/AllPassDelay.cpp
	src/BiquadFilter.cpp
	src/Cathedral.cpp
	src/Chamber.cpp
	src/Comb.cpp
	src/Crusher.cpp
	src/DelayBuffer.cpp
	src/Device.cpp
	src/Echo.cpp
	src/Envelope.cpp
	src/Falcon.cpp
	src/Helpers.cpp
	src/Leveller.cpp
	src/MxcsrFlagGuard.cpp
	src/ResampleBuffer.cpp
	src/SamplePlayer.cpp
	src/Scissor.cpp
	src/Slaughter.cpp
	src/Smasher.cpp
	src/Specimen.cpp
	src/StateVariableFilter.cpp
	src/SynthDevice.cpp
	src/Thunder.cpp
	src/Twister.cpp
	)

if(WIN32)
	target_sources(WaveSabreCore PRIVATE
		include/WaveSabreCore/GmDls.h
		src/GmDls.cppà)

	target_link_libraries(WaveSabreCore PUBLIC Msacm32.lib)
endif()

target_include_directories(WaveSabreCore PUBLIC include)

if(MSVC)
	target_compile_options(WaveSabreCore
		PUBLIC $<$<CONFIG:MinSizeRel>:/GS- /Gy /Zi /GL /GR->
		PRIVATE $<$<CONFIG:MinSizeRel>:/Oi /fp:fast>)

	set_property(TARGET WaveSabreCore APPEND_STRING PROPERTY STATIC_LIBRARY_FLAGS_MINSIZEREL
		" /LTCG")

	if(MSVC_VERSION GREATER 1900)
		target_compile_options(WaveSabreCore PUBLIC
			$<$<CONFIG:MinSizeRel>:/Zc:sizedDealloc->)
	endif()
else()
	# assuming GCC or clang for now

	#set(CMAKE_CXX_FLAGS "-g -Og")
	# stuff to make the code smaller
	set_property(TARGET WaveSabreCore PROPERTY INTERPROCEDURAL_OPTIMIZATION True) # enable LTO
	set(CMAKE_CXX_FLAGS "-g -O2 -ffast-math -fno-exceptions -fno-rtti -fno-stack-protector -fno-stack-check -fno-unwind-tables -fno-asynchronous-unwind-tables -fomit-frame-pointer -fno-threadsafe-statics -march=nocona -ffunction-sections -fdata-sections -Wl,--gc-sections")
endif()
