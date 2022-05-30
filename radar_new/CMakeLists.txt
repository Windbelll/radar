# project name
PROJECT(RADAR_NEW)
# requirement of cmake version
cmake_minimum_required(VERSION 3.16)
# set the directory of executable files
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake_module)
# find required
# find_package(ZED 3 REQUIRED)
find_package(OpenCV REQUIRED)
find_package(CUDA 11.1 REQUIRED)
find_package(Glog REQUIRED)
find_package(Threads)
# directory of headers
include_directories(${CUDA_INCLUDE_DIRS})
# include_directories(${ZED_INCLUDE_DIRS})
include_directories(${OpenCV_INCLUDE_DIRS})

include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/include/hardware)
# include_directories(${PROJECT_SOURCE_DIR}/include/ZedCamera)
include_directories(${PROJECT_SOURCE_DIR}/include/utilities)
include_directories(${PROJECT_SOURCE_DIR}/include/mindvision)
include_directories(${PROJECT_SOURCE_DIR}/include/mindvision/sdk)
# name of executable file and path of source file
set(SOURCE ${PROJECT_SOURCE_DIR}/main.cpp 
                                            # ${PROJECT_SOURCE_DIR}/src/ZedCamera/MyCamera.cpp
                                            # ${PROJECT_SOURCE_DIR}/src/ZedCamera/SaveDepth.cpp
                                            # ${PROJECT_SOURCE_DIR}/src/ZedCamera/common.cpp
                                            ${PROJECT_SOURCE_DIR}/src/yolo.cpp
                                            ${PROJECT_SOURCE_DIR}/src/map.cpp
                                            ${PROJECT_SOURCE_DIR}/src/com_judge.cpp
                                            ${PROJECT_SOURCE_DIR}/src/hardware/serial_device.cpp
                                            ${PROJECT_SOURCE_DIR}/include/mindvision/mindvision.cpp
                                            )
add_executable(RADAR_NEW ${SOURCE})
# directory of library
# link_directories(${ZED_LIBRARY_DIR})
link_directories(${OpenCV_LIBRARY_DIRS})
link_directories(${CUDA_LIBRARY_DIRS})
# libraries

target_link_libraries(RADAR_NEW ${OpenCV_LIBS}
                                # ${ZED_LIBRARIES} 
                                ${CUDA_LIBRARY} 
                                ${CUDA_CUDART_LIBRARY}
                                ${GLOG_LIBRARY}
                                ${CMAKE_THREAD_LIBS_INIT}
                                MVSDK
                                )