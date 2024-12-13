project(example)

find_package(glfw3 CONFIG REQUIRED)

add_executable(example
	src/entry.c
	src/game.c

	src/game.h
)

target_compile_definitions(example PRIVATE GLFW_INCLUDE_NONE)

target_include_directories(example PRIVATE
    ${CMAKE_SOURCE_DIR}/zeta_framework_3/include
    ${CMAKE_SOURCE_DIR}/zf3_common/include
    ${CMAKE_SOURCE_DIR}/vendor/glad/include
)

target_link_libraries(example PRIVATE glfw zeta_framework_3)

add_dependencies(example zf3_asset_packer)

add_custom_command(TARGET example POST_BUILD
	COMMAND $<TARGET_FILE:zf3_asset_packer> ${CMAKE_CURRENT_SOURCE_DIR}/assets ${CMAKE_CURRENT_BINARY_DIR}
)
