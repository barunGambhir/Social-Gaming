add_subdirectory(gtest)

add_executable(runAllTests
  ClientTest.cpp
  ServerTest.cpp
  # You can create new source files for your tests and add them here,
  # or you can add your tests to Dummy.cpp. Either way will work.
)

target_link_libraries(runAllTests
  PRIVATE
    GTest::gmock GTest::gtest GTest::gtest_main
    networking
)

add_test(NAME AllTests
  COMMAND ${CMAKE_CURRENT_BINARY_DIR}/runAllTests
)

