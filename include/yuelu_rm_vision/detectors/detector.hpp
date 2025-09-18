#pragma once

#include "yuelu_rm_vision/common/types.hpp"
#include <opencv2/opencv.hpp>
#include <vector>

namespace rm_vision {

// 抽象基类 (接口)
class Detector {
public:
    // 纯虚函数，定义了检测器的核心功能
    // 输入一帧图像，返回检测到的所有装甲板
    virtual std::vector<Armor> detect(const cv::Mat& frame) = 0;

    // 虚析构函数，保证子类能够正确地被销毁
    virtual ~Detector() = default;
};

} // namespace rm_vision