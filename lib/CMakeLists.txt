#one set of library configurations each

#libdummy.so
add_library(dummy SHARED libdummy.c)
target_link_libraries(dummy PRIVATE m)
target_include_directories(dummy PRIVATE "${PROJECT_BINARY_DIR}"
    "${PROJECT_BINARY_DIR}" 
    "${PROJECT_SOURCE_DIR}/color"
)

#libmandelqb.so
add_library(mandelqb SHARED libmandelquadbrute.c)
#target_link_libraries(mandelqb PRIVATE)
target_include_directories(mandelqb PRIVATE 
    "${PROJECT_BINARY_DIR}"
    "${PROJECT_SOURCE_DIR}/color"
)

#libbwpng.so
add_library(bwpng SHARED libbwpng.c)
target_link_libraries(bwpng PRIVATE png z)
target_include_directories(bwpng PRIVATE 
    "${PROJECT_BINARY_DIR}"
    "${PROJECT_SOURCE_DIR}/color"
)

#libminimal.so
add_library(minimal SHARED libminimalout.c)
#target_link_libraries(minimal PRIVATE color)
target_include_directories(minimal PRIVATE 
    "${PROJECT_BINARY_DIR}"
    "${PROJECT_SOURCE_DIR}/color"
)

#libcolorpng.so
add_library(colorpng SHARED libcolorpng.c)
#set_property(TARGET colorpng PROPERTY POSITION_INDEPENDENT_CODE ON)
target_link_libraries(colorpng PRIVATE png z color)
target_include_directories(colorpng PRIVATE 
    "${PROJECT_BINARY_DIR}" 
    "${PROJECT_SOURCE_DIR}/color"
)
