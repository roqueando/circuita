cmake_minimum_required(VERSION 3.27)
project(heximula LANGUAGES C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_STANDARD 23)
set(LIB_NAME core)

include(FetchContent)
set(FETCHCONTENT_QUIET FALSE)

# this is for libconfig
option(BUILD_EXAMPLES OFF)
option(BUILD_TESTS ON)
option(BUILD_FUZZERS OFF)
option(BUILD_CXX OFF)

#set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples
#set(BUILD_GAMES    OFF CACHE BOOL "" FORCE) # don't build the supplied example games

add_executable(heximula src/main.c)
target_link_libraries(heximula PUBLIC ${LIB_NAME})

include(packages/main.cmake)

add_subdirectory(src)
#FetchContent_Declare(
#    raylib
#    GIT_REPOSITORY "https://github.com/raysan5/raylib.git"
#    GIT_TAG "5.5"
#    GIT_PROGRESS TRUE
#    GIT_SHALLOW TRUE
#)
#
#FetchContent_MakeAvailable(raylib)

#set(CLAY_INCLUDE_SDL2_EXAMPLES OFF CACHE BOOL "" FORCE)
#set(CLAY_INCLUDE_ALL_EXAMPLES OFF CACHE BOOL "" FORCE)

#add_subdirectory(packages/clay clay-build)


#target_include_directories(lamina PUBLIC
#  "${CMAKE_SOURCE_DIR}/packages/clay"
#  "${CMAKE_SOURCE_DIR}/packages/clay/include"
#  "${CMAKE_SOURCE_DIR}/packages/clay/renderers/raylib"
#)

#target_link_libraries(lamina PUBLIC raylib)

#set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}")
#set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")
#
#
#add_custom_command(
#        TARGET lamina POST_BUILD
#        COMMAND ${CMAKE_COMMAND} -E copy_directory
#        ${CMAKE_CURRENT_SOURCE_DIR}/resources
#        ${CMAKE_CURRENT_BINARY_DIR}/resources)
add_subdirectory(test)
