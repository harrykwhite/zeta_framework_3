project(zf3_common)

add_library(zf3_common STATIC
    src/zf3c_logging.c
    src/zf3c_math.c
    src/zf3c_mem.c
    src/zf3c_misc.c

    include/zf3c.h
)

target_include_directories(zf3_common PRIVATE include)
