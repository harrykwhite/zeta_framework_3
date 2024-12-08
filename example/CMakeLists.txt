project(example)

find_package(glfw3 CONFIG REQUIRED)

get_filename_component(PARENT_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR} PATH)

add_executable(example
	src/entry.c
	${PARENT_SRC_DIR}/vendor/glad/src/glad.c
)

target_compile_definitions(example PRIVATE GLFW_INCLUDE_NONE)

target_include_directories(example PRIVATE
    ${PARENT_SRC_DIR}/zf3/include
    ${PARENT_SRC_DIR}/zf3_common/include
    ${PARENT_SRC_DIR}/vendor/glad/include
)

target_link_libraries(example PRIVATE glfw zf3)

add_dependencies(example zf3_asset_packer)

add_custom_command(TARGET example POST_BUILD
	COMMAND $<TARGET_FILE:zf3_asset_packer> ${CMAKE_CURRENT_SOURCE_DIR}/assets ${CMAKE_CURRENT_BINARY_DIR}
)
