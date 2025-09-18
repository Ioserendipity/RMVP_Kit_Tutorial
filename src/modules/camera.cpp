#include "yuelu_rm_vision/modules/camera.hpp"
#include <iostream>

namespace rm_vision {

// 为基类 Camera 的虚析构函数提供一个定义体
rm_vision::Camera::~Camera() {}

// ==================== USBCamera 完整实现 ====================
USBCamera::USBCamera(int device_index) 
    : device_index_(device_index) {}

// --- 这是之前缺失的函数实现 ---
bool USBCamera::open() {
    cap_.open(device_index_);
    if (!cap_.isOpened()) {
        std::cerr << "ERROR: Failed to open USB camera at index " << device_index_ << std::endl;
        return false;
    }
    return true;
}

// --- 这是之前缺失的函数实现 ---
bool USBCamera::is_open() const {
    return cap_.isOpened();
}

bool USBCamera::get_frame(cv::Mat& frame) {
    if (!is_open()) {
        return false;
    }
    return cap_.read(frame);
}
// ==========================================================


// ==================== VideoCamera 完整实现 ===================
VideoCamera::VideoCamera(const std::string& video_path) 
    : video_path_(video_path) {}

bool VideoCamera::open() {
    cap_.open(video_path_);
    if (!cap_.isOpened()) {
        std::cerr << "ERROR: Failed to open video file at " << video_path_ << std::endl;
        return false;
    }
    return true;
}

bool VideoCamera::is_open() const {
    return cap_.isOpened();
}

// 在 src/modules/camera.cpp 中
bool VideoCamera::get_frame(cv::Mat& frame) {
    if (!is_open()) {
        frame.release(); // 确保在这种情况下frame是空的
        return false;
    }
    
    if (cap_.read(frame)) {
        return true; // 成功读取一帧
    } else {
        // 读取失败，意味着视频结束
        std::cout << "[VideoCamera] Video finished. Replaying..." << std::endl;
        cap_.set(cv::CAP_PROP_POS_FRAMES, 0); // 重置到第0帧
        
        // 再次尝试读取第一帧
        if (cap_.read(frame)) {
            return true; // 重播成功
        } else {
            // 如果连第一帧都读不出来了，说明视频文件有问题
            std::cerr << "ERROR: Failed to read frame after replaying." << std::endl;
            frame.release(); // 确保返回一个空帧
            return false; // 返回失败
        }
    }
}

} // namespace rm_vision