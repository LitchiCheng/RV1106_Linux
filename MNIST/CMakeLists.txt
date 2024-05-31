cmake_minimum_required(VERSION 3.10)

set (CMAKE_C_COMPILER "/mnt/d/luckfox-pico/tools/linux/toolchain/arm-rockchip830-linux-uclibcgnueabihf/bin/arm-rockchip830-linux-uclibcgnueabihf-gcc")
set (CMAKE_CXX_COMPILER "/mnt/d/luckfox-pico/tools/linux/toolchain/arm-rockchip830-linux-uclibcgnueabihf/bin/arm-rockchip830-linux-uclibcgnueabihf-g++")

project(rtsp_mnist)

file(REMOVE_RECURSE ${CMAKE_CURRENT_SOURCE_DIR}/rtsp_mnist_test)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/rtsp_mnist_test)

set(OUTPUTPATH ${CMAKE_CURRENT_SOURCE_DIR}/rtsp_mnist_test)   

add_definitions(-DRV1106_1103)

link_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib)

set(CMAKE_INSTALL_RPATH "$ORIGIN/lib")

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../sources/3rdparty/ 3rdparty.out)

set(OpenCV_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lib/cmake/opencv4")
find_package(OpenCV REQUIRED)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_executable(${PROJECT_NAME}  
               ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cc
               ${CMAKE_CURRENT_SOURCE_DIR}/src/luckfox_mpi.cc 
               ${CMAKE_CURRENT_SOURCE_DIR}/src/mnist.cc 
               )

set(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -Wl,-rpath-link,${CMAKE_CURRENT_SOURCE_DIR}/lib:/usr/lib")

target_link_libraries(${PROJECT_NAME}  
                    ${LIBRKNNRT}
                    ${OpenCV_LIBS}
                    rockiva
                    sample_comm
                    rockit
                    rockchip_mpp
                    rkaiq
                    pthread
                    m
                    dl
                    rtsp
                    rga
                    Threads::Threads
                    )


target_include_directories(${PROJECT_NAME} PRIVATE                        
                           ${CMAKE_CURRENT_SOURCE_DIR}
                           ${CMAKE_CURRENT_SOURCE_DIR}/../sources/3rdparty/rknpu2/include
                           ${OpenCV_INCLUDE_DIRS}
                           ${CMAKE_CURRENT_SOURCE_DIR}/common 
                           ${CMAKE_CURRENT_SOURCE_DIR}/common/isp3.x   
                           ${CMAKE_CURRENT_SOURCE_DIR}/include
                           ${CMAKE_CURRENT_SOURCE_DIR}/include/rkaiq
                           ${CMAKE_CURRENT_SOURCE_DIR}/include/rkaiq/uAPI2
                           ${CMAKE_CURRENT_SOURCE_DIR}/include/rkaiq/common
                           ${CMAKE_CURRENT_SOURCE_DIR}/include/rkaiq/xcore
                           ${CMAKE_CURRENT_SOURCE_DIR}/include/rkaiq/algos
                           ${CMAKE_CURRENT_SOURCE_DIR}/include/rkaiq/iq_parser
                           ${CMAKE_CURRENT_SOURCE_DIR}/include/rkaiq/iq_parser_v2
                           ${CMAKE_CURRENT_SOURCE_DIR}/include/rkaiq/smartIr
                           ${CMAKE_CURRENT_SOURCE_DIR}/include/rknn 
                          )

                 

set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/rtsp_mnist_test")
file(GLOB RKNN_FILES "${CMAKE_CURRENT_SOURCE_DIR}/model/mnist.rknn")
install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX})
install(FILES ${RKNN_FILES} DESTINATION ${CMAKE_INSTALL_PREFIX}/model)