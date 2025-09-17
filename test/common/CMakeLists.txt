set(COMMON_TESTS "")

function(add_common_test name)
    cmake_parse_arguments(ADD_COMMON_TEST "HEADER_ONLY;MODULE" "" "" ${ARGN})
    
    set(sources ${name}.cpp ${ADD_COMMON_TEST_UNPARSED_ARGUMENTS})
    set(libs common GTest::gtest_main)
    
    add_executable(${name} ${sources})
    set_target_properties(${name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/test")
    target_link_libraries(${name} ${libs})
    target_include_directories(${name} PRIVATE ${RA_INCLUDE_DIRS})
    message(STATUS "Adding common test ${name} with sources ${sources}")
    gtest_discover_tests(${name})
    set(COMMON_TESTS ${COMMON_TESTS} ${name} PARENT_SCOPE)
endfunction()


add_common_test(utilities_test)
add_custom_target(common-tests
    DEPENDS ${COMMON_TESTS}
)
