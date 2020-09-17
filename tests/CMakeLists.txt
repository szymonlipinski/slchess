
file(GLOB UNIT_TEST_SOURCE_LIST
  "*.hpp"
  "*.cpp"
  )

message("Tests -> ${UNIT_TEST_SOURCE_LIST}")

set(TARGET_NAME slchess-tests)

add_executable(
  ${TARGET_NAME}
  main.cpp
  ${UNIT_TEST_SOURCE_LIST}
)


add_test(
  NAME ${TARGET_NAME}
  COMMAND ${TARGET_NAME}
)