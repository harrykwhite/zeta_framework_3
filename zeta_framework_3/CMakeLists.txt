project(zeta_framework_3)

find_package(glfw3 CONFIG REQUIRED)
find_package(OpenAL CONFIG REQUIRED)

add_library(zeta_framework_3 STATIC
    src/zf3_game.c
    src/zf3_window.c
    src/zf3_assets.c
    src/zf3_rendering.c
    src/zf3_audio.c
    src/zf3_utils.c
    ${CMAKE_SOURCE_DIR}/vendor/glad/src/glad.c

    include/zf3_public.h
    include/zf3_local.h
    ${CMAKE_SOURCE_DIR}/vendor/glad/include/glad/glad.h
    ${CMAKE_SOURCE_DIR}/vendor/glad/include/KHR/khrplatform.h
)

target_compile_definitions(zeta_framework_3 PRIVATE GLFW_INCLUDE_NONE)

target_include_directories(zeta_framework_3 PRIVATE
	include
	${CMAKE_SOURCE_DIR}/zf3_common/include
	${CMAKE_SOURCE_DIR}/vendor/glad/include
)

target_link_libraries(zeta_framework_3 PRIVATE zf3_common glfw OpenAL::OpenAL)
