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
