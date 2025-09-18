#pragma once

#include "yuelu_rm_vision/detectors/detector.hpp"

namespace rm_vision {

class TraditionalDetector : public Detector {
public:
    // 构造函数，用于初始化各种参数
    TraditionalDetector();

    // 实现基类的 detect 接口
    std::vector<Armor> detect(const cv::Mat& frame) override;

private:
    // 私有辅助函数，让代码结构更清晰
    cv::Mat preprocess(const cv::Mat& frame);
    std::vector<cv::RotatedRect> find_light_bars(const cv::Mat& binary_img);
    std::vector<Armor> match_armors(const std::vector<cv::RotatedRect>& light_bars);

    // --- 算法参数 ---
    int blue_threshold_; // 蓝色通道阈值
    int red_threshold_;  // 红色通道阈值
    
    // 灯条筛选参数
    double min_light_aspect_ratio_;
    double max_light_aspect_ratio_;
    double min_light_area_;
    
    // 装甲板匹配参数
    double max_angle_diff_;
    double max_y_diff_ratio_;
    double max_x_diff_ratio_;
};

} // namespace rm_vision