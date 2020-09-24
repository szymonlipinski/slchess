
file(GLOB UNIT_TEST_SOURCE_LIST
  "*.hpp"
  "*.cpp"
  )

set(TARGET_NAME slchess-tests)

add_executable(
  ${TARGET_NAME}
  ${UNIT_TEST_SOURCE_LIST}
)
target_link_libraries(${TARGET_NAME} ${LIBRARY_NAME})

target_include_directories(
  ${TARGET_NAME}
  PUBLIC ${CMAKE_BINARY_DIR}/sources
)

add_test(
  NAME ${TARGET_NAME}
  COMMAND ${TARGET_NAME}
)

target_compile_options(${TARGET_NAME} PUBLIC ${COMPILER_FLAGS})
