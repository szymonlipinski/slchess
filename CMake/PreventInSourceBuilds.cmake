# This function will prevent in-source builds
function(AssureOutOfSourceBuilds)
  # make sure the user doesn't play dirty with symlinks
  get_filename_component(srcdir "${CMAKE_SOURCE_DIR}" REALPATH)
  get_filename_component(bindir "${CMAKE_BINARY_DIR}" REALPATH)

  # disallow in-source builds
  if("${srcdir}" STREQUAL "${bindir}")
    message("######################################################")
    message("# The in-source builds are forbidden. ")
    message("# run ./run_cmake.sh instead ")
    message("######################################################")
    message(FATAL_ERROR "Quitting configuration")
  endif()
endfunction()

AssureOutOfSourceBuilds()
