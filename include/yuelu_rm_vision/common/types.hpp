#pragma once // 防止头文件被重复包含

#include <vector>
#include <string>
#include <opencv2/opencv.hpp>

namespace rm_vision {

// 定义装甲板颜色
enum class ArmorColor {
    BLUE,
    RED,
    UNKNOWN
};

// 装甲板对象：这是检测器的输出
struct Armor {
    std::vector<cv::Point2f> corners; // 装甲板灯条的四个角点 (2D像素坐标)
    std::string id;                   // 装甲板ID (e.g., "1", "3", "Sentry")
    float confidence;                 // 检测到的置信度
    ArmorColor color;                 // 颜色
};

// 目标对象：这是姿态解算器的输出和预测器的输入/输出
struct Target {
    cv::Point3f position;             // 目标在相机坐标系下的3D位置 (单位: mm)
    cv::Point3f velocity;             // 目标在相机坐标系下的3D速度 (单位: mm/s)
    std::string id;                   // 目标ID，与Armor对应
};

} // namespace rm_vision