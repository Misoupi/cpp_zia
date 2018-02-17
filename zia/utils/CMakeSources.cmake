### Sources for the Zia utils module

set(MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/utils)

set(MODULE_PUBLIC_HEADERS
        ${MODULE_PATH}/BufferUtils.hpp
        ${MODULE_PATH}/Automaton.hpp
        ${MODULE_PATH}/WeakCallback.hpp
        ${MODULE_PATH}/HTTPLengthExtractor.hpp
        )

set(MODULE_PRIVATE_HEADERS "")

set(MODULE_SOURCES ${MODULE_PUBLIC_HEADERS} ${MODULE_PRIVATE_HEADERS})
