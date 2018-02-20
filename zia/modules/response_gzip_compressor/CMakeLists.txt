add_library(response_gzip_compressor SHARED ResponseGZipCompressor.hpp ResponseGZipCompressor.cpp)

find_package(Boost 1.66 COMPONENTS iostreams REQUIRED)
find_package(ZLIB)

set_target_properties(response_gzip_compressor
        PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/modules"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/modules"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/modules"
        LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/modules"
        LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/modules"
        RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/modules"
        RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/modules"
        )

target_link_libraries(response_gzip_compressor Core::Lib Zia::Utils Boost::iostreams ${ZLIB_LIBRARIES})
target_include_directories(response_gzip_compressor PRIVATE ${CMAKE_SOURCE_DIR}/zia/utils)
