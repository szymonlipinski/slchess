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
