project("WaveSabre")

cmake_minimum_required(VERSION 3.11)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if(MSVC)
	# disable exceptions globally (will be added back for VSTs)
	string(REPLACE "/EHsc" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
endif()

set(VSTSDK3_DIR "./Vst3.x/" CACHE PATH "VSTSDK location")

option(BUILD_DEPRECATED "Build deprecated VSTs" OFF)
if(BUILD_DEPRECATED)
	add_compile_definitions(BUILD_DEPRECATED)
endif()

# shared code
add_subdirectory(WaveSabreCore)
add_subdirectory(WaveSabrePlayerLib)

# binaries
add_subdirectory(Tests/PlayerTest)
add_subdirectory(WaveSabreStandAlonePlayer)

# VSTs
if(VSTSDK3_DIR)
	add_subdirectory(WaveSabreVstLib)
	add_subdirectory(Vsts)
endif()
