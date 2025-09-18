cmake_minimum_required(VERSION 3.10)
project(RMVP_Kit_Tutorial_1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenCV REQUIRED)

# 在add_executable中加入我们所有需要的源文件
add_executable(rmvp_kit 
    src/main.cpp
    src/modules/camera.cpp
    src/modules/detectors/traditional_detector.cpp
    src/modules/detectors/yolo_detector.cpp  # <-- 新增 yolo_detector.cpp
)

target_include_directories(rmvp_kit PUBLIC 
    ${PROJECT_SOURCE_DIR}/include
    ${OpenCV_INCLUDE_DIRS}
)

target_link_libraries(rmvp_kit 
    ${OpenCV_LIBS}
)