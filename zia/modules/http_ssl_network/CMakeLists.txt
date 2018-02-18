add_library(http_ssl_network SHARED
        HTTPSSLConnection.hpp
        RoundRobinIOContextPool.cpp
        RoundRobinIOContextPool.hpp
        HTTPSSLNetwork.cpp
        HTTPSSLNetwork.hpp
        )

find_package(OpenSSL REQUIRED)
find_package(Boost 1.66 COMPONENTS system thread REQUIRED)

set_target_properties(http_ssl_network
        PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/modules"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/modules"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/modules"
        LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/modules"
        LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/modules"
        RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/modules"
        RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/modules"
        )

target_link_libraries(http_ssl_network Core::Log Core::Lib Zia::Utils Boost::thread Boost::system ${OPENSSL_LIBRARIES})
target_include_directories(http_ssl_network PRIVATE ${CMAKE_SOURCE_DIR}/zia/utils)
