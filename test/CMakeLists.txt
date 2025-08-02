enable_testing()

add_executable(test_resistor
    test_resistor.c
)

target_link_libraries(test_resistor
    PRIVATE ${LIB_NAME}
)

target_include_directories(test_resistor
    PRIVATE ${CMAKE_SOURCE_DIR}/include
)

add_test(
    NAME TestResistor
    COMMAND test_resistor
)
