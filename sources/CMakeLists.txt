
set(
  SOURCES
  config.hpp
  bitboard.hpp
  bitboard.cpp
)

add_library(${LIBRARY_NAME} SHARED ${SOURCES})

add_executable(${BINARY_NAME}-bin ${SOURCES} slchess.cpp)
target_link_libraries(${BINARY_NAME}-bin ${LIBRARY_NAME})

set_target_properties(
  ${BINARY_NAME}-bin
  PROPERTIES OUTPUT_NAME ${BINARY_NAME}
)