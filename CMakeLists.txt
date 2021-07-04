project("WaveSabre")

set (CMAKE_CXX_STANDARD 11)
cmake_minimum_required(VERSION 3.11)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if(MSVC)
	# disable exceptions globally (will be added back for VSTs)
	string(REPLACE "/EHsc" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
endif()

set(VSTSDK3_DIR "${CMAKE_SOURCE_DIR}/Vst3.x/" CACHE PATH "VSTSDK location")

# shared code
if(MSVC)
add_subdirectory(MSVCRT)
endif()
add_subdirectory(WaveSabreCore)
if(MSVC)
add_subdirectory(WaveSabrePlayerLib)

# binaries
add_subdirectory(Tests/PlayerTest)
add_subdirectory(WaveSabreStandAlonePlayer)
endif()

# VSTs
if(VSTSDK3_DIR)
	add_subdirectory(WaveSabreVstLib)
	add_subdirectory(Vsts)
endif()
