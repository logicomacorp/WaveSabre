add_library(msvcrt SHARED IMPORTED GLOBAL)
set_target_properties(msvcrt PROPERTIES IMPORTED_IMPLIB ${CMAKE_CURRENT_SOURCE_DIR}/msvcrt.lib)
