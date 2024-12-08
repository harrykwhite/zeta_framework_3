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
    ${PARENT_SRC_DIR}/vendor/glad/include
)

target_link_libraries(example PRIVATE glfw zf3)
