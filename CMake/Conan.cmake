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
