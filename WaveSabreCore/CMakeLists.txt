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
	include/WaveSabreCore/Devices.h
	include/WaveSabreCore/Echo.h
	include/WaveSabreCore/Envelope.h
	include/WaveSabreCore/Falcon.h
	include/WaveSabreCore/GmDls.h
	include/WaveSabreCore/GsmSample.h
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
	include/WaveSabreCore/Twister.h
	include/WaveSabreCore/Deprecated/Thunder.h
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
	src/GmDls.cpp
	src/GsmSample.cpp
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
	src/Twister.cpp
	src/Deprecated/Thunder.cpp)

target_link_libraries(WaveSabreCore Msacm32.lib)
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
endif()
