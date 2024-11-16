cmake_minimum_required (VERSION 3.25)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

include(Vcpkg)

# Setup the project
project(noise VERSION 1.0 LANGUAGES CXX)

# Set the global settings to use in the project
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_COMPILE_WARNING_AS_ERROR ON)

# Define installation directories variables using GNU Standard.
include(GNUInstallDirs)

set(NOISE_OUTPUT_DIR ${CMAKE_BINARY_DIR}/_output)

block()
    get_property(NOISE_IS_MULTI_CONFIG GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
    if (NOISE_IS_MULTI_CONFIG)
        # Setup build output directories
        foreach(TYPE ${CMAKE_CONFIGURATION_TYPES})
            string(TOUPPER ${TYPE} TYPE_UPPER)
            set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${TYPE_UPPER} ${NOISE_OUTPUT_DIR}/${TYPE}/${CMAKE_INSTALL_LIBDIR} PARENT_SCOPE)
            set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${TYPE_UPPER} ${NOISE_OUTPUT_DIR}/${TYPE}/${CMAKE_INSTALL_LIBDIR} PARENT_SCOPE)
            set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${TYPE_UPPER} ${NOISE_OUTPUT_DIR}/${TYPE}/${CMAKE_INSTALL_BINDIR} PARENT_SCOPE)
        endforeach()
    else()
        set(ARCHIVE_OUTPUT_DIRECTORY ${NOISE_OUTPUT_DIR}/${CMAKE_BUILD_TYPE}/${CMAKE_INSTALL_LIBDIR})
        set(LIBRARY_OUTPUT_DIRECTORY ${NOISE_OUTPUT_DIR}/${CMAKE_BUILD_TYPE}/${CMAKE_INSTALL_LIBDIR})
        set(RUNTIME_OUTPUT_DIRECTORY ${NOISE_OUTPUT_DIR}/${CMAKE_BUILD_TYPE}/${CMAKE_INSTALL_BINDIR})

        # Setup build output directories
        set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${ARCHIVE_OUTPUT_DIRECTORY} PARENT_SCOPE)
        set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${LIBRARY_OUTPUT_DIRECTORY} PARENT_SCOPE)
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${RUNTIME_OUTPUT_DIRECTORY} PARENT_SCOPE)
    endif()
endblock()

# Set a default property for all targets so they are built by ALL
define_property(DIRECTORY PROPERTY DIR_IN_ALL INHERITED)
set_directory_properties(PROPERTIES DIR_IN_ALL ON)

# Define the targets
add_subdirectory(noise)
add_subdirectory(app)
