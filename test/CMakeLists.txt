find_package(GTest CONFIG REQUIRED)

# For Windows: Prevent overriding the parent project's compiler/linker settings
if(WIN32)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
        # let's also hide the gtest targets from the IDE
    set_target_properties(GTest::gtest PROPERTIES FOLDER extern)
    set_target_properties(GTest::gtest_main PROPERTIES FOLDER extern)
    set_target_properties(GTest::gmock PROPERTIES FOLDER extern)
    set_target_properties(GTest::gmock_main PROPERTIES FOLDER extern)
endif()

include(GoogleTest)

add_library(mocks INTERFACE)
target_include_directories(mocks INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(mocks INTERFACE GTest::gmock)

add_subdirectory(common)
add_subdirectory(login)

add_custom_target(tests
    DEPENDS common-tests login-tests
)
