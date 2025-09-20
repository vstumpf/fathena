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

add_subdirectory(common)

add_custom_target(tests
    DEPENDS common-tests
)
