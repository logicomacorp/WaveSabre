project("WaveSabre")

cmake_minimum_required(VERSION 3.11)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if(MSVC)
	# disable exceptions globally (will be added back for VSTs)
	string(REPLACE "/EHsc" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
endif()

option(VSTSDK3_DIR "VSTSDK location" "./Vst3.x/")

# shared code
add_subdirectory(WaveSabreCore)
add_subdirectory(WaveSabrePlayerLib)
add_subdirectory(WaveSabreVstLib)

# binaries
add_subdirectory(Tests/PlayerTest)
add_subdirectory(WaveSabreStandAlonePlayer)
add_subdirectory(Vsts)
