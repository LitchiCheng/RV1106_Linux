cmake_minimum_required(VERSION 3.15)

# set (CMAKE_C_COMPILER "arm-linux-gnueabihf-gcc")
# set (CMAKE_CXX_COMPILER "arm-linux-gnueabihf-g++")

set (CMAKE_C_COMPILER "/mnt/d/luckfox-pico/tools/linux/toolchain/arm-rockchip830-linux-uclibcgnueabihf/bin/arm-rockchip830-linux-uclibcgnueabihf-gcc")
set (CMAKE_CXX_COMPILER "/mnt/d/luckfox-pico/tools/linux/toolchain/arm-rockchip830-linux-uclibcgnueabihf/bin/arm-rockchip830-linux-uclibcgnueabihf-g++")


project(captureJpgApp)

add_executable(${PROJECT_NAME})
aux_source_directory(. sources)
aux_source_directory(tools sources)
target_sources(${PROJECT_NAME} PUBLIC ${sources})

target_include_directories(${PROJECT_NAME} PUBLIC
                           ../utility
                           tools
                           )