set(SOURCES conf-test.cpp ${CMAKE_SOURCE_DIR}/zia/core/ConfigLoader.cpp)
CREATE_UNIT_TEST(conf-test Core: "${SOURCES}")
include_directories(${CMAKE_SOURCE_DIR}/zia/core)
include_directories(${CMAKE_SOURCE_DIR}/api/)
include_directories(${CMAKE_SOURCE_DIR}/vendor/json/single_include)

if (LINUX)
    target_link_libraries(conf-test Core::Config stdc++fs)
endif(LINUX)

if(APPLE)
    target_link_libraries(conf-test Core::Config /usr/local/lib/libc++experimental.a)
endif(APPLE)

if (WIN32)
    target_link_libraries(conf-test Core::Config)
endif(WIN32)
