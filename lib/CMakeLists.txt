#one set of library configurations each

#libmandelqb.so
add_library(mandelqb SHARED libmandelquadbrute.c)
target_include_directories(mandelqb PRIVATE 
    "${PROJECT_BINARY_DIR}"
    "${PROJECT_SOURCE_DIR}/color"
)

#libjuliaqb.so
add_library(juliaqb SHARED libjuliaquadbrute.c)
target_include_directories(juliaqb PRIVATE 
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
target_include_directories(minimal PRIVATE 
    "${PROJECT_BINARY_DIR}"
    "${PROJECT_SOURCE_DIR}/color"
)

#libcolorpng.so
add_library(colorpng SHARED libcolorpng.c)
target_link_libraries(colorpng PRIVATE png z color)
target_include_directories(colorpng PRIVATE 
    "${PROJECT_BINARY_DIR}" 
    "${PROJECT_SOURCE_DIR}/color"
)
