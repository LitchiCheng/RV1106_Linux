cmake_minimum_required(VERSION 3.10)

set (CMAKE_C_COMPILER "/mnt/d/luckfox-pico/tools/linux/toolchain/arm-rockchip830-linux-uclibcgnueabihf/bin/arm-rockchip830-linux-uclibcgnueabihf-gcc")
set (CMAKE_CXX_COMPILER "/mnt/d/luckfox-pico/tools/linux/toolchain/arm-rockchip830-linux-uclibcgnueabihf/bin/arm-rockchip830-linux-uclibcgnueabihf-g++")

project(rkmpi-test)

add_definitions(-DRV1106_1103)

link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../sources/lib)

set(CMAKE_INSTALL_RPATH "$ORIGIN/lib")

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../sources/3rdparty/ 3rdparty.out)


set(OpenCV_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../sources/lib/cmake/opencv4")
find_package(OpenCV REQUIRED)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)


add_executable(${PROJECT_NAME}  
               ${CMAKE_CURRENT_SOURCE_DIR}/rkmpi-test.cc
               ${CMAKE_CURRENT_SOURCE_DIR}/luckfox_mpi.cc 
               )

target_link_libraries(${PROJECT_NAME}  
                    ${OpenCV_LIBS}
                    rockiva
                    sample_comm
                    rockit
                    rockchip_mpp
                    rkaiq
                    pthread
                    rtsp
                    rga
                    Threads::Threads
                    )


target_include_directories(${PROJECT_NAME} PRIVATE                        
                           ${CMAKE_CURRENT_SOURCE_DIR}
                           ${CMAKE_CURRENT_SOURCE_DIR}/../sources/3rdparty/rknpu2/include
                           ${OpenCV_INCLUDE_DIRS}
                           ${CMAKE_CURRENT_SOURCE_DIR}/../sources/common 
                           ${CMAKE_CURRENT_SOURCE_DIR}/../sources/common/isp3.x   
                           ${CMAKE_CURRENT_SOURCE_DIR}/../sources/include
                           ${CMAKE_CURRENT_SOURCE_DIR}/../sources/3rdparty/rknpu2/include
                           ${CMAKE_CURRENT_SOURCE_DIR}/../sources/include/rkaiq
                           ${CMAKE_CURRENT_SOURCE_DIR}/../sources/include/rkaiq/uAPI2
                           ${CMAKE_CURRENT_SOURCE_DIR}/../sources/include/rkaiq/common
                           ${CMAKE_CURRENT_SOURCE_DIR}/../sources/include/rkaiq/xcore
                           ${CMAKE_CURRENT_SOURCE_DIR}/../sources/include/rkaiq/algos
                           ${CMAKE_CURRENT_SOURCE_DIR}/../sources/include/rkaiq/iq_parser
                           ${CMAKE_CURRENT_SOURCE_DIR}/../sources/include/rkaiq/iq_parser_v2
                           ${CMAKE_CURRENT_SOURCE_DIR}/../sources/include/rkaiq/smartIr 
                          )