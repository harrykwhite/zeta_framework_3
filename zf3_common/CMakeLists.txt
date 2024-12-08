project(zf3_common)

add_library(zf3_common STATIC
    src/zf3_common_math.c
    include/zf3_common.h
)

target_include_directories(zf3_common PRIVATE include)
