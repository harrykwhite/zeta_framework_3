project(zf3_common)

add_library(zf3_common STATIC
    src/zf3c_logging.cpp
    src/zf3c_math.cpp
    src/zf3c_mem.cpp
    src/zf3c_misc.cpp

    include/zf3c.h
)

target_include_directories(zf3_common PRIVATE include)
