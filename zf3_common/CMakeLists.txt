project(zf3_common)

add_library(zf3_common STATIC
    src/zf3c_debugging.cpp
    src/zf3c_math.cpp
    src/zf3c_misc.cpp

    include/zf3c.h
    include/zf3c_debugging.h
    include/zf3c_assets.h
    include/zf3c_math.h
    include/zf3c_misc.h
)

target_include_directories(zf3_common PRIVATE include)
