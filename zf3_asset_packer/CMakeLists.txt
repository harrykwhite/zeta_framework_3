get_filename_component(PARENT_DIR "${CMAKE_CURRENT_LIST_DIR}" DIRECTORY)

project(zf3_asset_packer)

find_package(cJSON CONFIG REQUIRED)
find_package(Freetype REQUIRED)

add_executable(zf3_asset_packer
	src/zf3ap_main.cpp
	src/zf3ap_textures.cpp
	${PARENT_DIR}/vendor/stb_image/src/stb_image.cpp

	src/zf3ap.h
)

target_include_directories(zf3_asset_packer PRIVATE
	${PARENT_DIR}/zf3_common/include
	${PARENT_DIR}/vendor/stb_image/include
)

target_link_libraries(zf3_asset_packer PRIVATE zf3_common cjson Freetype::Freetype)
