cmake_minimum_required(VERSION 3.10)

set (CMAKE_C_COMPILER "/mnt/d/luckfox-pico/tools/linux/toolchain/arm-rockchip830-linux-uclibcgnueabihf/bin/arm-rockchip830-linux-uclibcgnueabihf-gcc")
set (CMAKE_CXX_COMPILER "/mnt/d/luckfox-pico/tools/linux/toolchain/arm-rockchip830-linux-uclibcgnueabihf/bin/arm-rockchip830-linux-uclibcgnueabihf-g++")

project(rknn_yolov5_realtime)

file(REMOVE_RECURSE ${CMAKE_CURRENT_SOURCE_DIR}/output)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/output)

set(OUTPUTPATH ${CMAKE_CURRENT_SOURCE_DIR}/output)

if (ENABLE_ASAN)
	message(STATUS "BUILD WITH ADDRESS SANITIZER")
	set (CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
	set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
	set (CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
endif ()

add_definitions(-DRV1106_1103)
set(rknpu2_yolov5_file rknpu2/yolov5_rv1106_1103.cc)
#dma
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../sources/3rdparty/allocator/dma)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../sources/3rdparty/ 3rdparty.out)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/utils/ utils.out)

file(GLOB SRCS ${CMAKE_CURRENT_SOURCE_DIR}/*.cc)


add_executable(${PROJECT_NAME}
    main.cc
    postprocess.cc
    ../v4l2/tools/v4l2CapPicTool.cpp
    ${rknpu2_yolov5_file}
)

target_link_libraries(${PROJECT_NAME}
    imageutils
    fileutils
    imagedrawing    
    ${LIBRKNNRT}
)

# if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
#     set(THREADS_PREFER_PTHREAD_FLAG ON)
#     find_package(Threads REQUIRED)
#     target_link_libraries(${PROJECT_NAME} Threads::Threads)
# endif()

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${LIBRKNNRT_INCLUDES}
    ../v4l2/tools
    ./3rdparty/rknpu2/include
)

install(TARGETS ${PROJECT_NAME} DESTINATION ${OUTPUTPATH})
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/model/bus.jpg DESTINATION ${OUTPUTPATH}/model)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/model/coco_80_labels_list.txt DESTINATION ${OUTPUTPATH}/model)
file(GLOB RKNN_FILES "${CMAKE_CURRENT_SOURCE_DIR}/model/*.rknn")
install(FILES ${RKNN_FILES} DESTINATION ${OUTPUTPATH}/model)