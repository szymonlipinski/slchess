
set(
  SOURCES
  config.hpp
  bitboard.hpp
  bitboard.cpp
)



add_library(${LIBRARY_NAME} STATIC ${SOURCES})

target_compile_options(${LIBRARY_NAME} PUBLIC ${COMPILER_FLAGS})

add_executable(${BINARY_NAME}-bin ${SOURCES} slchess.cpp)
target_link_libraries(${BINARY_NAME}-bin ${LIBRARY_NAME})

set_target_properties(
  ${BINARY_NAME}-bin
  PROPERTIES OUTPUT_NAME ${BINARY_NAME}
)
target_compile_options(${BINARY_NAME}-bin PUBLIC ${COMPILER_FLAGS})



