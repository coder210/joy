set(EXECUTABLE_NAME "joy_beat")
message(STATUS "EXECUTABLE_NAME = '${EXECUTABLE_NAME}'")

add_executable(${EXECUTABLE_NAME})

file(GLOB SRC_FILES "*.h" "*.c" "*.hpp" "*.cpp")

set(PROJECT_FILES
    ${SRC_FILES}
)

target_sources(${EXECUTABLE_NAME}
    PRIVATE
        ${PROJECT_FILES}
)

set_target_properties(${EXECUTABLE_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

if(MSVC)
    set_target_properties(${EXECUTABLE_NAME} PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>"
    )
endif()

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${PROJECT_FILES})

target_link_libraries(${EXECUTABLE_NAME} PUBLIC SDL3::SDL3 joy)
