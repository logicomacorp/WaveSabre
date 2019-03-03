project("WaveSabre")

cmake_minimum_required(VERSION 3.11)

# shared code
add_subdirectory(WaveSabreCore)
add_subdirectory(WaveSabrePlayerLib)
add_subdirectory(WaveSabreVstLib)

# binaries
add_subdirectory(Tests/PlayerTest)
add_subdirectory(WaveSabreStandAlonePlayer)
add_subdirectory(Vsts)
