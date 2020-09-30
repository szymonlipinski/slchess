# ---------------------------------------------------------
# Enables ccache
#
# Creates an option ENABLE_CCACHE
# ---------------------------------------------------------
function(EnableCCache)
option(ENABLE_CCACHE "Use ccache" ON)
find_program(CCACHE ccache)

if (ENABLE_CCACHE)
  if (CCACHE)
    set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE})
  endif ()
endif ()
endfunction()
# ---------------------------------------------------------
EnableCCache()
