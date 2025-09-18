#pragma once // 保持不变

#include <opencv2/opencv.hpp>
#include <string>

namespace rm_vision {

// 抽象相机基类 (接口) - 保持不变
class Camera {
public:
    virtual bool open() = 0;
    virtual bool is_open() const = 0;
    virtual bool get_frame(cv::Mat& frame) = 0;

    // === 修改这里 ===
    // 不再使用 = default
    // 只留下声明
    virtual ~Camera(); 
    // ===============
};

// USB相机的具体实现 - 保持不变
class USBCamera : public Camera {
public:
    explicit USBCamera(int device_index);
    bool open() override;
    bool is_open() const override;
    bool get_frame(cv::Mat& frame) override;
private:
    cv::VideoCapture cap_;
    int device_index_;
};

// ==================== 新增部分 开始 ====================

// 从视频文件读取的相机实现
class VideoCamera : public Camera {
public:
    // 构造函数，传入视频文件的路径
    explicit VideoCamera(const std::string& video_path);
    
    // 覆盖基类的纯虚函数
    bool open() override;
    bool is_open() const override;
    bool get_frame(cv::Mat& frame) override;

private:
    cv::VideoCapture cap_;
    std::string video_path_;
};

// ==================== 新增部分 结束 ====================

} // namespace rm_vision