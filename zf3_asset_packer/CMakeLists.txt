project(zf3_asset_packer)

find_package(cJSON CONFIG REQUIRED)
find_package(Freetype REQUIRED)

add_executable(zf3_asset_packer
	src/zf3_asset_packer_main.c
	src/zf3_asset_packer_textures.c
	${CMAKE_SOURCE_DIR}/vendor/stb_image/src/stb_image.c

	src/zf3_asset_packer_shared.h
)

target_include_directories(zf3_asset_packer PRIVATE
	${CMAKE_SOURCE_DIR}/zf3_common/include
	${CMAKE_SOURCE_DIR}/vendor/stb_image/include
)

target_link_libraries(zf3_asset_packer PRIVATE zf3_common cjson Freetype::Freetype)
