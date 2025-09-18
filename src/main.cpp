#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <opencv2/opencv.hpp>
#include "yuelu_rm_vision/modules/camera.hpp"
#include "yuelu_rm_vision/detectors/yolo_detector.hpp"
#include "yuelu_rm_vision/common/constants.hpp"

// 绘图函数
void draw_detections(cv::Mat& frame, const std::vector<rm_vision::Armor>& detections) {
    for (const auto& det : detections) {
        if (det.corners.size() == 4) {
            // 使用左上角和右下角来绘制矩形
            cv::rectangle(frame, cv::Point(det.corners[0]), cv::Point(det.corners[2]), cv::Scalar(0, 255, 0), 2); // 使用绿色
            
            try {
                int class_id = std::stoi(det.id.substr(3));
                std::string label;
                if (class_id >= 0 && class_id < rm_vision::COCO_CLASSES.size()) {
                    label = rm_vision::COCO_CLASSES[class_id];
                } else {
                    label = "unknown";
                }
                label += ": " + cv::format("%.2f", det.confidence);
                
                cv::Point text_origin = det.corners[0];
                text_origin.y -= 5;
                cv::putText(frame, label, text_origin, cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(0, 255, 255), 2);
            } catch (const std::exception& e) {
                // 静默处理解析失败的情况
            }
        }
    }
}

int main() {
    std::string video_path = "../data/videos/traffic.mp4";
    auto camera = std::make_unique<rm_vision::VideoCamera>(video_path);
    if (!camera || !camera->open()) { 
        std::cerr << "ERROR: Could not open video file." << std::endl;
        return -1; 
    }
    std::cout << "[INFO] Camera source opened successfully." << std::endl;

    // 加载 YOLOv5n 模型
    std::string model_path = "../assets/models/yolov5n.onnx";
    // YOLOv5对置信度的要求可以稍高一些
    auto detector = std::make_unique<rm_vision::YoloDetector>(model_path, 0.45f, 0.5f);

    std::cout << "==========================================" << std::endl;
    std::cout << "[INFO] Using Detector: YOLOv5" << std::endl;
    std::cout << "       Model Path: " << model_path << std::endl;
    std::cout << "==========================================" << std::endl;


    cv::Mat frame;
    while (true) {
        if (!camera->get_frame(frame) || frame.empty()) {
            std::cout << "[INFO] End of video stream." << std::endl;
            break; 
        }

        // 不再需要手动resize，YOLOv5的预处理会自己处理
        auto detections = detector->detect(frame);
        
        draw_detections(frame, detections);

        cv::imshow("YOLOv5 Detection Result", frame);
        
        if (cv::waitKey(1) == 'q') {
            break;
        }
    }
    
    cv::destroyAllWindows();
    std::cout << "[INFO] Program finished." << std::endl;
    return 0;
}