project("WaveSabre")

cmake_minimum_required(VERSION 3.11)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# shared code
add_subdirectory(WaveSabreCore)
add_subdirectory(WaveSabrePlayerLib)
add_subdirectory(WaveSabreVstLib)

# binaries
add_subdirectory(Tests/PlayerTest)
add_subdirectory(WaveSabreStandAlonePlayer)
add_subdirectory(Vsts)
