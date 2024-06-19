add_library(color SHARED color.c reference.c)
target_include_directories(color INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
