# ${CMAKE_SOURCE_DIR}/datastructures/quadtree/test/CMakeLists.txt
add_library(
  catch_main
  OBJECT
  ${CMAKE_SOURCE_DIR}/thirdparty/Catch2/catch.cpp
)

target_link_libraries(
  catch_main
  PRIVATE
  CATCH2::header_only
)

set(
  TEST_FILES
  testPrintNode
  testQuadTree
)

foreach(TEST_FILE IN LISTS TEST_FILES)
  add_executable(
    ${TEST_FILE}
    ${TEST_FILE}.cpp
    $<TARGET_OBJECTS:catch_main>
  )

  target_link_libraries(
    ${TEST_FILE}
    PRIVATE
    glm
    fmt::fmt
    options::options
    CATCH2::header_only
  )

  add_test(
    ${TEST_FILE}
    ${TEST_FILE}
  )
endforeach()

