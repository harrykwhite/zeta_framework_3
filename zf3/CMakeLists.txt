project(zf3)

find_package(glfw3 CONFIG REQUIRED)
find_package(OpenAL CONFIG REQUIRED)

get_filename_component(PARENT_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR} PATH)

add_library(zf3 STATIC
    src/zf3_game.c
    src/zf3_window.c
    src/zf3_assets.c
    src/zf3_rendering.c
    src/zf3_audio.c
    src/zf3_scenes.c
    src/zf3_utils.c
    ${PARENT_SRC_DIR}/vendor/glad/src/glad.c

    include/zf3.h
    ${PARENT_SRC_DIR}/vendor/glad/include/glad/glad.h
    ${PARENT_SRC_DIR}/vendor/glad/include/KHR/khrplatform.h
)

target_compile_definitions(zf3 PRIVATE GLFW_INCLUDE_NONE)

target_include_directories(zf3 PRIVATE
	include
	${PARENT_SRC_DIR}/zf3_common/include
	${PARENT_SRC_DIR}/vendor/glad/include
)

target_link_libraries(zf3 PRIVATE zf3_common glfw OpenAL::OpenAL)
