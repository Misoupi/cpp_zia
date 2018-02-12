add_library(upload_file SHARED UploadFile.cpp UploadFile.hpp)

    # hack to get around the "Debug" and "Release" directories cmake tries to add on Windows

set_target_properties(upload_file
        PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/modules"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/modules"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/modules"
        LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/modules"
        LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/modules"
        RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/modules"
        RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/modules"
        )

target_link_libraries(upload_file Core::Lib)