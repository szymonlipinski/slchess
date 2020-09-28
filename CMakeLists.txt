cmake_minimum_required(VERSION 3.16)

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

# ---------------------------------------------------------
# CCACHE
# ---------------------------------------------------------
option(ENABLE_CCACHE "Use ccache" ON)
find_program(CCACHE ccache)

if (ENABLE_CCACHE)
  if (CCACHE)
    set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE})
  endif ()
endif ()
# ---------------------------------------------------------

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

# -------------------------------------------------------------------
# C++ version.
# -------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# -------------------------------------------------------------------

# -------------------------------------------------------------------
# specify where to place binaries and libraries
# -------------------------------------------------------------------
include(GNUInstallDirs)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_BINDIR})

set(BINARY_NAME ${CMAKE_PROJECT_NAME})
set(LIBRARY_NAME "${CMAKE_PROJECT_NAME}")
# -------------------------------------------------------------------


# ---------------------------------------------------------
# CPP CHECK
# ---------------------------------------------------------
option(ENABLE_CPPCHECK "Enable testing with cppcheck" FALSE)
find_program(CPPCHECK cppcheck)
if (ENABLE_CPPCHECK)

  if (CPPCHECK)
    set(CMAKE_CXX_CPPCHECK
      ${CPPCHECK}
      --suppress=syntaxError
      --enable=all
      --inconclusive)
  else ()
    message(SEND_ERROR "cppcheck requested but executable not found")
  endif ()
endif ()


# ---------------------------------------------------------
# CLANG TIDY
# ---------------------------------------------------------
option(ENABLE_CLANG_TIDY "Enable testing with clang-tidy" ON)

find_program(CLANGTIDY clang-tidy)
if (ENABLE_CLANG_TIDY)
  if (CLANGTIDY)
    set(CMAKE_CXX_CLANG_TIDY ${CLANGTIDY})
  else ()
    message(SEND_ERROR "clang-tidy requested but executable not found")
  endif ()
endif ()
# ---------------------------------------------------------

# -------------------------------------------------------------------
# CONAN
# -------------------------------------------------------------------

if (NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
  message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
  file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/master/conan.cmake"
    "${CMAKE_BINARY_DIR}/conan.cmake")
endif ()
include(${CMAKE_BINARY_DIR}/conan.cmake)

#conan_add_remote(NAME bincrafters
#  URL https://api.bintray.com/conan/bincrafters/public-conan)

conan_check(REQUIRED)

conan_cmake_run(
  CONANFILE conanfile.txt

  BASIC_SETUP
  CMAKE_TARGETS
  OPTIONS

  CONAN_COMMAND ${CONAN_CMD}

  BUILD missing
  GENERATORS cmake_find_package
)
# -------------------------------------------------------------------

set(
  COMPILER_FLAGS
  -Werror -Wall -Wextra
  -pedantic-errors
)

# -Wall -Wextra -Wshadow -Wnon-virtual-dtor -pedantic

# -------------------------------------------------------------------
# Add subdirectories.
# -------------------------------------------------------------------
add_subdirectory(${SOURCES_DIR})

enable_testing()
add_subdirectory(${TESTS_DIR})
# -------------------------------------------------------------------

# -------------------------------------------------------------------
# Prints command to use to change the varname which should be an option
# -------------------------------------------------------------------
function(message_change varname)
  if (${${varname}})
    set(negvar OFF)
  else ()
    set(negvar ON)
  endif ()
  message("  --> change with -D" ${varname} "=" ${negvar})
endfunction()
# -------------------------------------------------------------------


# -------------------------------------------------------------------
# Summary.
# -------------------------------------------------------------------
message("###################################################")
message("                      Summary                      ")
message("###################################################")
message("Build type:                       ${CMAKE_BUILD_TYPE}")
message("Install prefix:                   ${CMAKE_INSTALL_PREFIX}")

message("Use CCache:                       ${ENABLE_CCACHE}")
message_change(ENABLE_CCACHE)
message("CCache executable:                ${CCACHE}")

message("Use CPPCheck:                     ${ENABLE_CPPCHECK}")
message_change(ENABLE_CPPCHECK)
message("CPPCheck executable:              ${CPPCHECK}")

message("Use ClangTidy:                    ${ENABLE_CLANG_TIDY}")
message_change(ENABLE_CLANG_TIDY)
message("ClangTidy executable:             ${CLANGTIDY}")


message("###################################################")
message("Using C++ standard:               ${CMAKE_CXX_STANDARD}")
message("Compiler extensions enabled:      ${CMAKE_CXX_EXTENSIONS}")
message("C++ standard required:            ${CMAKE_CXX_STANDARD_REQUIRED}")
message("###################################################")
message("Version:                          ${PROJECT_VERSION}")
message("###################################################")
# -------------------------------------------------------------------
