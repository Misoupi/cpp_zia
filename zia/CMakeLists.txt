set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

set(API_PATH ${CMAKE_SOURCE_DIR}/api)
include_directories(${API_PATH})
include_directories(${CMAKE_SOURCE_DIR}/vendor/json/single_include)

set(API_SOURCES
        ${API_PATH}/api/conf.h
        ${API_PATH}/api/http.h
        ${API_PATH}/api/module.h
        ${API_PATH}/api/net.h
        )

set(ZIA_CORE_PATH core)

set(ZIA_SOURCES
        ${API_SOURCES}
        ${ZIA_CORE_PATH}/Core.hpp
        ${ZIA_CORE_PATH}/Core.cpp
        ${ZIA_CORE_PATH}/ConfigLoader.hpp
        ${ZIA_CORE_PATH}/ConfigLoader.cpp
        ${ZIA_CORE_PATH}/main.cpp
        )

add_executable(zia ${ZIA_SOURCES})

target_link_libraries(zia Core::Lib Core::Log)
