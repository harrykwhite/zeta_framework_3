cmake_minimum_required(VERSION 3.28)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(zeta_framework_3)

add_subdirectory(zeta_framework_3)
add_subdirectory(zf3_asset_packer)
add_subdirectory(zf3_common)
add_subdirectory(example)
