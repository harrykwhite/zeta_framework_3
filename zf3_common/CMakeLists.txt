project(zf3_common)

add_library(zf3_common STATIC
    src/zf3_common_logging.c
    src/zf3_common_math.c
    src/zf3_common_mem.c
    src/zf3_common_misc.c

    include/zf3_common.h
)

target_include_directories(zf3_common PRIVATE include)
