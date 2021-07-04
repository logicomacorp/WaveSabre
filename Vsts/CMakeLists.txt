set(VSTDIR "" CACHE PATH "VST system directory")

find_path(LIBGSM_INCLUDE_DIR NAMES gsm.h
  PATHS
  /usr/local/include/gsm
  /usr/local/include
  /usr/include/gsm
  /usr/include
  )

find_library(LIBGSM_LIBRARY NAMES gsm
  PATHS
  /usr/local/lib
  /usr/lib
  )

if(LIBGSM_INCLUDE_DIR AND LIBGSM_LIBRARY)
  add_definitions(-DHAVE_LIBGSM)
endif(LIBGSM_INCLUDE_DIR AND LIBGSM_LIBRARY)

if(APPLE)
    find_library(CARBON_LIBRARY Carbon)
    find_library(COCOA_LIBRARY Cocoa)
    find_library(AUDIO_TOOLBOX_LIBRARY AudioToolbox)
    mark_as_advanced(CARBON_LIBRARY COCOA_LIBRARY)
    set(EXTRA_LIBS ${CARBON_LIBRARY} ${COCOA_LIBRARY} ${AUDIO_TOOLBOX_LIBRARY} ${LIBGSM_LIBRARY})
    file(GLOB RESOURCE_FILES ../Data/*.png)
endif(APPLE)

file(GLOB children RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/*)
foreach(child ${children})
	if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${child})
		file(GLOB sources ${CMAKE_CURRENT_SOURCE_DIR}/${child}/*.h)
		aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/${child} sources)

		if(WIN32)
			file(WRITE "${CMAKE_BINARY_DIR}/${child}.def"
				"LIBRARY ${child}\nEXPORTS\nVSTPluginMain\nmain=VSTPluginMain")
			add_library(${child} SHARED
				${sources} ../Data/data.rc ${CMAKE_BINARY_DIR}/${child}.def)
			set_property(TARGET ${child} APPEND_STRING PROPERTY LINK_FLAGS_MINSIZEREL
				" /LTCG")
			set_property(TARGET ${child} PROPERTY FOLDER VSTs)
		endif(WIN32)

		if(APPLE)
			add_library(${child} MODULE ${sources} ${RESOURCE_FILES})
			set_target_properties(${child} PROPERTIES
			  BUNDLE true
			  BUNDLE_EXTENSION "vst"
			  XCODE_ATTRIBUTE_WRAPPER_EXTENSION "vst"
			  MACOSX_BUNDLE_INFO_PLIST "Data/Info.plist.in"
			  MACOSX_BUNDLE_BUNDLE_NAME "${child}"
			  MACOSX_BUNDLE_GUI_IDENTIFIER "io.logicoma.wavesabre.${child}"
			  MACOSX_BUNDLE_ICON_FILE ""
			  MACOSX_BUNDLE_SHORT_VERSION_STRING "1.0.0"
			  MACOSX_BUNDLE_COPYRIGHT "Copyright 2012-2021 WaveSabre Team"
			  XCODE_ATTRIBUTE_USE_HEADERMAP YES
			  RESOURCE "${RESOURCE_FILES}"
			)
			target_include_directories(${child} PUBLIC ${LIBGSM_INCLUDE_DIR})
			target_link_options(${child} PUBLIC "LINKER:-all_load")
		endif(APPLE)

		target_link_libraries(${child} WaveSabreCore WaveSabreVstLib ${EXTRA_LIBS})

		if(VSTDIR)
			add_custom_command(TARGET ${child} POST_BUILD
				COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${child}> ${VSTDIR})
		endif()
	endif()
endforeach()
