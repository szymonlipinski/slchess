
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


function(PrintSummary)

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
endfunction()

PrintSummary()
