project("WaveSabre" LANGUAGES C CXX CSharp)

# Enable C# Utilities also for the VS2013 AppVeyor stage.
cmake_policy(SET CMP0057 NEW)
include(CSharpUtilities)

# Note: The space suffix is required for the flag to work correctly.
set(CMAKE_CSharp_FLAGS "/langversion:latest ")

find_package(Git REQUIRED)
include(ExternalProject)

cmake_minimum_required(VERSION 3.11)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if(MSVC)
	# disable exceptions globally (will be added back for VSTs)
	string(REPLACE "/EHsc" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
endif()

set(BUILD_VST_PLUGINS ON CACHE BOOL "Opt out of building the VST plugins.")
set(BUILD_WAVESABRE_CONVERT ON CACHE BOOL "Do not build WaveSabreConvert.")
set(BUILD_WAVESABRE_CONVERT_TESTS ON CACHE BOOL "Do not build the WaveSabreConvert tests.")
set(BUILD_CONVERT_THE_FUCK ON CACHE BOOL "Do not build ConvertTheFuck.")
set(BUILD_PROJECT_MANAGER ON CACHE BOOL "Do not build the project manager.")
set(VSTSDK3_DIR "${PROJECT_SOURCE_DIR}/Vst3.x/" CACHE PATH "VSTSDK location.")
set(MONAD_FL_PARSER_DIR "${CMAKE_CURRENT_BINARY_DIR}/Monad.FLParser.Source" CACHE PATH "Monad FL parser location.")

# Download and unpack VST3 SDK
set(DOWNLOAD_VST3SDK OFF CACHE BOOL "Download and unpack VST3 SDK automatically.")
if(${BUILD_VST_PLUGINS} AND ${DOWNLOAD_VST3SDK})
	find_file(VST3SDK_TEST name public.sdk HINTS ${VSTSDK3_DIR})
	if(${VST3SDK_TEST} MATCHES VST3SDK_TEST-NOTFOUND)
		message(STATUS "VST3 SDK not found. Will download.")
		file(DOWNLOAD https://web.archive.org/web/20200502121517/https://www.steinberg.net/sdk_downloads/vstsdk366_27_06_2016_build_61.zip vstsdk366_27_06_2016_build_61.zip SHOW_PROGRESS)
		file(ARCHIVE_EXTRACT INPUT vstsdk366_27_06_2016_build_61.zip DESTINATION vstsdk366_27_06_2016_build_61 VERBOSE)
		file(GLOB VST3SDK_FILES "${CMAKE_CURRENT_BINARY_DIR}/vstsdk366_27_06_2016_build_61/VST3\ SDK/*")
		file(COPY ${VST3SDK_FILES} DESTINATION ${VSTSDK3_DIR})
	endif()
endif()

# Download and unpack the Monad FL Parser
set(DOWNLOAD_MONAD_FL_PARSER OFF CACHE BOOL "Download the Monad FL parser binary automatically.")
if(${BUILD_WAVESABRE_CONVERT} AND ${DOWNLOAD_MONAD_FL_PARSER})
	set(MONAD_FL_PARSER_BINARY ${CMAKE_CURRENT_BINARY_DIR}/Monad.FLParser/Release/Monad.FLParser.dll)
	find_file(MONAD_FL_PARSER_TEST NAME Project.cs HINTS ${MONAD_FL_PARSER_DIR})
	if(${MONAD_FL_PARSER_TEST} MATCHES MONAD_FL_PARSER_TEST-NOTFOUND)
		message(STATUS "Monad FL parser not found. Will download.")
		ExternalProject_Add(FLParser
			SOURCE_DIR ${MONAD_FL_PARSER_DIR}
			BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/Monad.FLParser
			GIT_REPOSITORY https://github.com/Kwarf/FLParser.git
			CONFIGURE_COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/WaveSabreConvert/FLParser/CMakeLists.txt ${MONAD_FL_PARSER_DIR}/CMakeLists.txt && ${CMAKE_COMMAND} -DCMAKE_GENERATOR_PLATFORM=${CMAKE_GENERATOR_PLATFORM} ${MONAD_FL_PARSER_DIR} -DMONAD_FL_PARSER_DIR=${MONAD_FL_PARSER_DIR}
			BUILD_COMMAND ${CMAKE_COMMAND} --build ${CMAKE_CURRENT_BINARY_DIR}/Monad.FLParser --config Release -- -m
			INSTALL_COMMAND ""
		)
	endif()
elseif(${BUILD_WAVESABRE_CONVERT})
	message(STATUS "Using existent FL parser repo.")
	add_subdirectory(WaveSabreConvert/FLParser)
	set(MONAD_FL_PARSER_BINARY ${CMAKE_CURRENT_BINARY_DIR}/WaveSabreConvert/FLParser/Release/Monad.FLParser.dll)
endif()

# shared code
add_subdirectory(MSVCRT)
add_subdirectory(WaveSabreCore)
add_subdirectory(WaveSabrePlayerLib)

# binaries
add_subdirectory(Tests/PlayerTest)
add_subdirectory(WaveSabreStandAlonePlayer)

# VSTs
if(${BUILD_VST_PLUGINS} AND VSTSDK3_DIR)
	add_subdirectory(WaveSabreVstLib)
	add_subdirectory(Vsts)
endif()

# Project file conversions
if(${BUILD_WAVESABRE_CONVERT})
	add_subdirectory(WaveSabreConvert)
endif()
if(${BUILD_WAVESABRE_CONVERT_TESTS})
	add_subdirectory(Tests/WaveSabreConvertTests)
endif()
if(${BUILD_CONVERT_THE_FUCK})
	add_subdirectory(ConvertTheFuck)
endif()

# Project manager
if(${BUILD_PROJECT_MANAGER})
	add_subdirectory(ProjectManager)
endif()
