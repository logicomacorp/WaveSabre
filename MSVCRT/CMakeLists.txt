add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/msvcrt.lib
                   MAIN_DEPENDENCY msvcrt.def
                   COMMAND lib.exe /machine:${CMAKE_CXX_COMPILER_ARCHITECTURE_ID} /def:${CMAKE_CURRENT_SOURCE_DIR}/msvcrt.def /out:${CMAKE_CURRENT_BINARY_DIR}/msvcrt.lib
                   COMMENT "Creating msvcrt.lib"
                   VERBATIM)
add_custom_target(build-msvcrt DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/msvcrt.lib)

add_library(msvcrt_dll SHARED IMPORTED GLOBAL)
set_target_properties(msvcrt_dll PROPERTIES IMPORTED_IMPLIB ${CMAKE_CURRENT_BINARY_DIR}/msvcrt.lib)
add_dependencies(msvcrt_dll build-msvcrt)

add_library(msvcrt STATIC mainCRTStartup.c)
target_link_libraries(msvcrt msvcrt_dll)
