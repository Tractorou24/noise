find_package(libassert QUIET REQUIRED)
find_package(cxxopts QUIET REQUIRED)

add_executable(noise-generator)
target_sources(noise-generator
    PRIVATE
        src/main.cpp
)

target_link_libraries(noise-generator
    PRIVATE
        noise
        libassert::assert
        cxxopts::cxxopts
)
