get_filename_component(PARENT_DIR "${CMAKE_CURRENT_LIST_DIR}" DIRECTORY)

project(zeta_framework_3)

find_package(glfw3 CONFIG REQUIRED)
find_package(OpenAL CONFIG REQUIRED)

add_library(zeta_framework_3 STATIC
    src/zf3_game.cpp
    src/zf3_window.cpp
    src/zf3_assets.cpp
    src/zf3_shader_progs.cpp
    src/zf3_renderer.cpp
    src/zf3_audio.cpp
    src/zf3_rand.cpp
    src/zf3_misc.cpp
    ${PARENT_DIR}/vendor/glad/src/glad.c

    include/zf3.h
    include/zf3_game.h
    include/zf3_window.h
    include/zf3_assets.h
    include/zf3_shader_progs.h
    include/zf3_renderer.h
    include/zf3_audio.h
    include/zf3_rand.h
    include/zf3_misc.h
    ${PARENT_DIR}/vendor/glad/include/glad/glad.h
    ${PARENT_DIR}/vendor/glad/include/KHR/khrplatform.h
)

target_include_directories(zeta_framework_3 PRIVATE
	include
	${PARENT_DIR}/zf3_common/include
	${PARENT_DIR}/vendor/glad/include
)

target_link_libraries(zeta_framework_3 PRIVATE zf3_common glfw OpenAL::OpenAL)

target_compile_definitions(zeta_framework_3 PRIVATE GLFW_INCLUDE_NONE)
