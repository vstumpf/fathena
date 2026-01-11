set(LOGIN_TESTS "")

function(add_login_test name)
    cmake_parse_arguments(ADD_LOGIN_TEST "HEADER_ONLY;MODULE" "" "" ${ARGN})
    
    set(sources ${name}.cpp ${ADD_LOGIN_TEST_UNPARSED_ARGUMENTS})
    set(libs login GTest::gtest_main mocks)
    
    add_executable(${name} ${sources})
    set_target_properties(${name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/test")
    target_link_libraries(${name} ${libs})
    target_include_directories(${name} PRIVATE ${RA_INCLUDE_DIRS})
    message(STATUS "Adding login test ${name} with sources ${sources}")
    gtest_discover_tests(${name})
    set(LOGIN_TESTS ${LOGIN_TESTS} ${name} PARENT_SCOPE)
endfunction()

add_subdirectory(accountdb)
add_subdirectory(ipbandb)

add_custom_target(login-tests
    DEPENDS ${LOGIN_TESTS}
)
