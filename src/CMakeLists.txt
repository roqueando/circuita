file(GLOB_RECURSE HEADER_LIST
  RELATIVE
  "${CMAKE_CURRENT_SOURCE_DIR}/include/core"
  CONFIGURE_DEPENDS
  "${CMAKE_cURRENT_SOURCE_DIR}/include/core/*.h"
)

file(GLOB_RECURSE SOURCE_LIST
  RELATIVE
  "${PROJECT_SOURCE_DIR}/src"
  "${PROJECT_SOURCE_DIR}/src/*.c"
)

message(STATUS ${SOURCE_LIST})

add_library(${LIB_NAME} STATIC
    core/microcontroller.c
    core/pin_manager.c
    cli/cli.c
    qemu/qemu_interface.c
)

target_include_directories(${LIB_NAME} PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(${LIB_NAME} PUBLIC libconfig)
