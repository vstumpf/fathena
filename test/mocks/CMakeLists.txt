add_library(mocks INTERFACE)
target_include_directories(mocks INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(mocks INTERFACE GTest::gmock)
