find_package(glm QUIET CONFIG REQUIRED)
find_package(PNG QUIET REQUIRED)
find_path(PNGPP_INCLUDE_DIRS "png++/image.hpp" QUIET REQUIRED)

add_library(noise STATIC)
target_sources(noise
    PUBLIC
        FILE_SET public_headers
        TYPE HEADERS
        FILES
            include/noise/noise.h
            include/noise/perlin.h
            include/noise/voronoi.h
        BASE_DIRS
            include
    PRIVATE
        src/perlin.cpp
        src/voronoi.cpp
)

target_link_libraries(noise 
    PUBLIC 
        glm::glm
    PRIVATE
        PNG::PNG
)
target_include_directories(noise PRIVATE ${PNGPP_INCLUDE_DIRS})
