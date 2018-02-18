add_library(http_network SHARED
        HTTPConnection.hpp
        RoundRobinIOContextPool.cpp
        RoundRobinIOContextPool.hpp
        HTTPNetwork.cpp
        HTTPNetwork.hpp
        )

find_package(Boost 1.66 COMPONENTS system thread REQUIRED)

set_target_properties(http_network
        PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/modules"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/modules"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/modules"
        LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/modules"
        LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/modules"
        RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/modules"
        RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/modules"
        )

target_link_libraries(http_network Core::Log Core::Lib Zia::Utils Boost::thread Boost::system)
target_include_directories(http_network PRIVATE ${CMAKE_SOURCE_DIR}/zia/utils)
