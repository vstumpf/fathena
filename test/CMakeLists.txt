cmake_policy(SET CMP0135 NEW)

include(FetchContent)
FetchContent_Declare(
  googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.17.0 # Or a specific commit hash
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
if(WIN32)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
        # let's also hide the gtest targets from the IDE
    set_target_properties(GTest::gtest PROPERTIES FOLDER extern)
    set_target_properties(GTest::gtest_main PROPERTIES FOLDER extern)
    set_target_properties(GTest::gmock PROPERTIES FOLDER extern)
    set_target_properties(GTest::gmock_main PROPERTIES FOLDER extern)
endif()

FetchContent_MakeAvailable(googletest)
include(GoogleTest)

add_subdirectory(common)

add_custom_target(tests
    DEPENDS common-tests
)
