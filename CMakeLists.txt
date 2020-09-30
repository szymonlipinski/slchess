cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

# -------------------------------------------------------------------
# Load additional modules from external files.
# -------------------------------------------------------------------
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMake ${CMAKE_MODULE_PATH})

# -------------------------------------------------------------------
# Set C++ version.
# -------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# -------------------------------------------------------------------

# -------------------------------------------------------------------
# Set version.
# -------------------------------------------------------------------
set(PROJECT_VERSION_MAJOR 0)
set(PROJECT_VERSION_MINOR 1)
set(PROJECT_VERSION_PATCH 0)

set(PROJECT_VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH})
# -------------------------------------------------------------------

# -------------------------------------------------------------------
# Set project name.
# -------------------------------------------------------------------
project(
  slchess
  VERSION ${PROJECT_VERSION}
  DESCRIPTION "A simple chess engine"
  LANGUAGES CXX
)

# -------------------------------------------------------------------
# Set basic directories names.
# -------------------------------------------------------------------
set(SOURCES_DIR ${PROJECT_SOURCE_DIR}/sources)
set(TESTS_DIR ${PROJECT_SOURCE_DIR}/tests)
# -------------------------------------------------------------------

# -------------------------------------------------------------------
# Configure the config file to have access to the version from the
# source files.
# -------------------------------------------------------------------
configure_file(
  "${SOURCES_DIR}/config.hpp.in"
  "${SOURCES_DIR}/config.hpp"
)
# -------------------------------------------------------------------

set(BINARY_NAME ${CMAKE_PROJECT_NAME})
set(LIBRARY_NAME "${CMAKE_PROJECT_NAME}")


include(PreventInSourceBuilds)
include(EnableCCache)
include(EnableCPPCheck)
include(ConfigureGnuDirs)
include(EnableClangTidy)
include(Conan)

# -------------------------------------------------------------------
# Add subdirectories.
# -------------------------------------------------------------------
add_subdirectory(${SOURCES_DIR})

enable_testing()
add_subdirectory(${TESTS_DIR})
# -------------------------------------------------------------------


include(PrintSummary)
