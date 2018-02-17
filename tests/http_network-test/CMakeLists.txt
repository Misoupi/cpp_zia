set(API_PATH ${CMAKE_SOURCE_DIR}/api)
include_directories(${API_PATH})
set(SOURCES http_network-test.cpp)
CREATE_UNIT_TEST(http_network-test Core: "${SOURCES}")
target_include_directories(http_network-test PRIVATE
        ${CMAKE_SOURCE_DIR}/zia/utils
        ${CMAKE_SOURCE_DIR}/zia/modules/http_network
        )
target_link_libraries(http_network-test Core::Lib Zia::Utils)
add_dependencies(http_network-test http_network)
