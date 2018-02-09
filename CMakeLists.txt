if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message(FATAL_ERROR "Prevented in-tree built. Please create a build directory outside of the source code and call cmake from there")
endif()

cmake_minimum_required(VERSION 3.9)

##! Prerequisites CTEST
enable_testing()

##! Project Name
project(zia)

##! Module Path
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/vendor/core-v2/cmake)
add_subdirectory(vendor/core-v2)
include(${CMAKE_MODULE_PATH}/library.cmake)

##! Project Module Path
set(ZIA_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake_config)

##! CMake Library Include
include(${CMAKE_MODULE_PATH}/library.cmake)

##! CMake Project Configuration include
include(${ZIA_MODULE_PATH}/${PROJECT_NAME}.cmake)

##! Project Options
option(ZIA_BUILD_TESTS "Build tests of the zia project" OFF)

add_subdirectory(zia)

##! Project Tests
if (ZIA_BUILD_TESTS)
    add_subdirectory(tests)
endif ()
