project("WaveSabre")

cmake_minimum_required(VERSION 3.11)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if(NOT WIN32)
	option(USE_PTHREADS "Use Pthreads for threading" OFF)
	option(ENABLE_APLAY "Enable aplay(1) backend for audio playback" ON)
	option(ENABLE_SDL2 "Enable SDL2_Audio backend for audio playback" ON)
endif()

if(MSVC)
	# disable exceptions globally (will be added back for VSTs)
	string(REPLACE "/EHsc" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
endif()

if(WIN32)
	set(VSTSDK3_DIR "./Vst3.x/" CACHE PATH "VSTSDK location")
endif()

# shared code
if(WIN32)
	add_subdirectory(MSVCRT)
endif()
add_subdirectory(WaveSabreCore)
add_subdirectory(WaveSabrePlayerLib)

# binaries
if(WIN32)
	add_subdirectory(Tests/PlayerTest)
endif()
add_subdirectory(WaveSabreStandAlonePlayer)

# VSTs
if(WIN32)
	if(VSTSDK3_DIR)
		add_subdirectory(WaveSabreVstLib)
		add_subdirectory(Vsts)
	endif()
endif()
