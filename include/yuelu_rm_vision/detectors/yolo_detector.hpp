#pragma once

#include "yuelu_rm_vision/detectors/detector.hpp"
#include <opencv2/dnn.hpp>
#include <string>
#include <vector>

namespace rm_vision {

class YoloDetector : public Detector {
public:
    YoloDetector(const std::string& model_path, float conf_threshold, float nms_threshold);

    std::vector<Armor> detect(const cv::Mat& frame) override;

private:
    cv::dnn::Net net_;
    float conf_threshold_;
    float nms_threshold_;
    int input_width_;
    int input_height_;
};

} // namespace rm_vision