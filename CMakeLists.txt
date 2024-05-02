#can't get cmake to build libcolorpng when color.* is in subdirectory color.
#passing oversubs variable has not worked so far.
#got the interface stuff in color/CMakeLists from exercise 3 in the cmake tut. However, maybe I need to learn more about using interfaces.


cmake_minimum_required(VERSION 3.5)

project(frascr)
project(frascr VERSION 2.0)
configure_file(options.h.in options.h)
include_directories(${PROJECT_SOURCE_DIR})
add_subdirectory(color)
list(APPEND "${PROJECT_SOURCE_DIR}/color")
#set(oversubs "")
#list(APPEND oversubs "${PROJECT_SOURCE_DIR}/color")
add_subdirectory(lib)
add_executable(frascr
  mand_v2.c
  debug.c
#  utils.c
  options.c
  libopen.c
#  color.c
)
target_link_libraries(frascr PUBLIC
  json-c
  m
  color
)
target_include_directories(frascr PUBLIC
  "${PROJECT_BINARY_DIR}"
)
