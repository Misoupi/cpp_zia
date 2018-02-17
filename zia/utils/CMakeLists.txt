include(CMakeSources.cmake)
set(MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})
CREATE_MODULE(Zia::Utils "${MODULE_SOURCES}" ${MODULE_PATH})
#target_link_libraries(Zia-Utils INTERFACE)