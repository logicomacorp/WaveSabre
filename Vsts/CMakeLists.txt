set(VSTDIR "" CACHE PATH "VST system directory")

file(GLOB children RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/*)
foreach(child ${children})
	if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${child})
		file(GLOB sources ${CMAKE_CURRENT_SOURCE_DIR}/${child}/*.h)
		aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/${child} sources)
		file(WRITE "${CMAKE_BINARY_DIR}/${child}.def"
			"LIBRARY ${child}\nEXPORTS\nVSTPluginMain\nmain=VSTPluginMain")
		add_library(${child} SHARED
			${sources} ../Data/data.rc ${CMAKE_BINARY_DIR}/${child}.def)
		target_link_libraries(${child} WaveSabreCore WaveSabreVstLib)
		set_property(TARGET ${child} APPEND_STRING PROPERTY LINK_FLAGS_MINSIZEREL
			" /LTCG")
		set_property(TARGET ${child} PROPERTY FOLDER VSTs)

		if(VSTDIR)
			add_custom_command(TARGET ${child} POST_BUILD
				COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${child}> ${VSTDIR})
		endif()
	endif()
endforeach()
