project("WaveSabre" LANGUAGES C CXX CSharp)

# Enable C# Utilities also for the VS2013 AppVeyor stage.
cmake_policy(SET CMP0057 NEW)
include(CSharpUtilities)
set(CMAKE_CSharp_FLAGS ${CMAKE_CSharp_FLAGS} "/langversion:6")

cmake_minimum_required(VERSION 3.11)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if(MSVC)
	# disable exceptions globally (will be added back for VSTs)
	string(REPLACE "/EHsc" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
endif()

set(VSTSDK3_DIR "${CMAKE_SOURCE_DIR}/Vst3.x/" CACHE PATH "VSTSDK location")

# Download and unpack VST3 SDK
set(DOWNLOAD_VST3SDK CACHE BOOL OFF)
if(${DOWNLOAD_VST3SDK})
	find_file(VST3SDK_TEST name public.sdk HINTS ${VSTSDK3_DIR})
	if(${VST3SDK_TEST} MATCHES VST3SDK_TEST-NOTFOUND)
		message(STATUS "VST3 SDK not found. Will download.")
		file(DOWNLOAD https://web.archive.org/web/20200502121517/https://www.steinberg.net/sdk_downloads/vstsdk366_27_06_2016_build_61.zip vstsdk366_27_06_2016_build_61.zip SHOW_PROGRESS)
		file(ARCHIVE_EXTRACT INPUT vstsdk366_27_06_2016_build_61.zip DESTINATION vstsdk366_27_06_2016_build_61 VERBOSE)
		file(GLOB VST3SDK_FILES "${CMAKE_CURRENT_BINARY_DIR}/vstsdk366_27_06_2016_build_61/VST3\ SDK/*")
		file(COPY ${VST3SDK_FILES} DESTINATION ${VSTSDK3_DIR})
	endif()
endif()

# shared code
add_subdirectory(MSVCRT)
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

# Project file conversions
add_subdirectory(WaveSabreConvert)
add_subdirectory(Tests/WaveSabreConvertTests)
add_subdirectory(ConvertTheFuck)

# Project manager
add_subdirectory(ProjectManager)
