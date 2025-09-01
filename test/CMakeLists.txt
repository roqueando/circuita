add_executable(test_basic test_basic.c)
target_link_libraries(test_basic PUBLIC core)

enable_testing()
add_test(NAME BasicTest COMMAND test_basic)
