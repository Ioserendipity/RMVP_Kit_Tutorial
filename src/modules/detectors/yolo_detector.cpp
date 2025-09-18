#include "yuelu_rm_vision/detectors/yolo_detector.hpp" // <-- [修复] 使用完整路径
#include "yuelu_rm_vision/common/constants.hpp"   // <-- [修复] 使用完整路径
#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp> // <-- 确保包含了OpenCV

namespace rm_vision {

YoloDetector::YoloDetector(const std::string& model_path, float conf_threshold, float nms_threshold)
    : conf_threshold_(conf_threshold), nms_threshold_(nms_threshold) {
    
    net_ = cv::dnn::readNetFromONNX(model_path);
    if (net_.empty()) {
        std::cerr << "FATAL: Failed to load ONNX model from " << model_path << std::endl;
    } else {
        std::cout << "[YoloDetector] DEBUG: YOLO model loaded successfully." << std::endl;
    }
    
    input_width_ = 640;
    input_height_ = 640;
}

std::vector<Armor> YoloDetector::detect(const cv::Mat& frame) {
    cv::Mat blob;
    cv::dnn::blobFromImage(frame, blob, 1.0/255.0, cv::Size(input_width_, input_height_), cv::Scalar(), true, false);

    net_.setInput(blob);
    std::vector<cv::Mat> outs;
    net_.forward(outs, net_.getUnconnectedOutLayersNames());

    std::vector<int> class_ids;
    std::vector<float> confidences;
    std::vector<cv::Rect> boxes;
    std::vector<Armor> armors;

    float x_factor = (float)frame.cols / input_width_;
    float y_factor = (float)frame.rows / input_height_;

    const float* data = (float*)outs[0].data;
    const int dimensions = 5 + COCO_CLASSES.size();
    const int rows = 25200;

    for (int i = 0; i < rows; ++i) {
        float confidence = data[4];
        if (confidence >= conf_threshold_) {
            const float* classes_scores = data + 5;
            cv::Mat scores(1, COCO_CLASSES.size(), CV_32F, (void*)classes_scores);
            cv::Point class_id_point;
            double max_class_score;
            cv::minMaxLoc(scores, 0, &max_class_score, 0, &class_id_point);

            if (max_class_score > 0.25) {
                confidences.push_back(confidence * (float)max_class_score);
                class_ids.push_back(class_id_point.x);
                float cx = data[0]; float cy = data[1]; float w = data[2]; float h = data[3];
                int left = (int)((cx - w / 2) * x_factor);
                int top = (int)((cy - h / 2) * y_factor);
                int width = (int)(w * x_factor);
                int height = (int)(h * y_factor);
                boxes.push_back(cv::Rect(left, top, width, height));
            }
        }
        data += dimensions;
    }

    std::vector<int> nms_result_indices;
    cv::dnn::NMSBoxes(boxes, confidences, conf_threshold_, nms_threshold_, nms_result_indices);

    for (int idx : nms_result_indices) {
        Armor armor;
        cv::Rect box = boxes[idx];
        armor.confidence = confidences[idx];
        armor.id = "id:" + std::to_string(class_ids[idx]);
        armor.color = ArmorColor::UNKNOWN;
        armor.corners.push_back(cv::Point2f(box.x, box.y));
        armor.corners.push_back(cv::Point2f(box.x + box.width, box.y));
        armor.corners.push_back(cv::Point2f(box.x + box.width, box.y + box.height));
        armor.corners.push_back(cv::Point2f(box.x, box.y + box.height));
        armors.push_back(armor);
    }
    
    return armors;
}

} // namespace rm_vision