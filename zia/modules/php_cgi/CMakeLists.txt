add_library(php_cgi SHARED
        PHPCGI.cpp
        PHPCGI.hpp)

find_package(Boost 1.66 COMPONENTS system thread filesystem REQUIRED)

set_target_properties(php_cgi
        PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/modules"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/modules"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/modules"
        LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/modules"
        LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/modules"
        RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/modules"
        RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/modules"
        )

target_link_libraries(php_cgi Core::Log Core::Lib Boost::thread Boost::filesystem Boost::system)
target_include_directories(php_cgi PRIVATE ${CMAKE_SOURCE_DIR}/zia/utils)
if (WIN32)
    add_custom_command(TARGET php_cgi POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_SOURCE_DIR}/modules/boost_chrono-vc141-mt-gd-x64-1_66.dll"
            "${CMAKE_SOURCE_DIR}/bin/boost_chrono-vc141-mt-gd-x64-1_66.dll")
    add_custom_command(TARGET php_cgi POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_SOURCE_DIR}/modules/boost_date_time-vc141-mt-gd-x64-1_66.dll"
            "${CMAKE_SOURCE_DIR}/bin/boost_date_time-vc141-mt-gd-x64-1_66.dll")
    add_custom_command(TARGET php_cgi POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_SOURCE_DIR}/modules/boost_filesystem-vc141-mt-gd-x64-1_66.dll"
            "${CMAKE_SOURCE_DIR}/bin/boost_filesystem-vc141-mt-gd-x64-1_66.dll")
    add_custom_command(TARGET php_cgi POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_SOURCE_DIR}/modules/boost_system-vc141-mt-gd-x64-1_66.dll"
            "${CMAKE_SOURCE_DIR}/bin/boost_system-vc141-mt-gd-x64-1_66.dll")
    add_custom_command(TARGET php_cgi POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_SOURCE_DIR}/modules/boost_thread-vc141-mt-gd-x64-1_66.dll"
            "${CMAKE_SOURCE_DIR}/bin/boost_thread-vc141-mt-gd-x64-1_66.dll")
endif (WIN32)
