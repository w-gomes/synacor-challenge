cmake_minimum_required(VERSION 3.16)

project(synacor LANGUAGES CXX)
include(cmake/standard_project_settings.cmake)
include(cmake/prevent_in_source_builds.cmake)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# add installed libraries
find_package(fmt REQUIRED)

# Link this 'library' to set the c++ standard / compile-time options requested
add_library(project_options INTERFACE)
target_compile_features(project_options INTERFACE cxx_std_20)
target_include_directories(project_options INTERFACE ${BOOST_ALGORITHM_INCLUDE_DIRS}
                                                     ${CMAKE_SOURCE_DIR}/external
                                                     ${CMAKE_SOURCE_DIR}/src
)

# sanitizer options if supported by compiler
include(cmake/Sanitizers.cmake)
enable_sanitizers(project_options)

# Link this 'library' to use the warnings specified in compiler_warnings.cmake
add_library(project_warnings INTERFACE)
include(cmake/compiler_warnings.cmake)
set_project_warnings(project_warnings)

# static analyzers
include(cmake/static_analyzers.cmake)

add_executable(synacor src/main.cpp
                       src/computer.cpp
)

target_link_libraries(
  synacor PRIVATE project_warnings 
                  project_options
                  fmt::fmt-header-only
)
