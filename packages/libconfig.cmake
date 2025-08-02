set(XXX_BUILD_EXAMPLES OFF)
set(XXX_BUILD_TESTS ON)
set(XXX_BUILD_FUZZERS OFF)
set(XXX_BUILD_CXX OFF)

FetchContent_Declare(
    libconfig
    GIT_REPOSITORY "https://github.com/hyperrealm/libconfig"
    GIT_TAG "v1.8.1"
    GIT_PROGRESS TRUE
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(libconfig)

if (libconfig_POPULATED)
  add_library(libconfig INTERFACE IMPORTED)
  target_include_directories(libconfig INTERFACE "${libconfig_SOURCE_DIR}/lib")
endif()
