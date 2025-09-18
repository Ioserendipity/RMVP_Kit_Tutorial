#include "yuelu_rm_vision/detectors/traditional_detector.hpp"
#include <vector>

namespace rm_vision {

// 在 traditional_detector.cpp 的构造函数中
TraditionalDetector::TraditionalDetector() {
    blue_threshold_ = 40;
    red_threshold_ = 40;
    
    // --- 恢复更严格的灯条筛选条件 ---
    min_light_aspect_ratio_ = 2.0;  // 灯条至少是 2:1 的长宽比
    max_light_aspect_ratio_ = 15.0;
    min_light_area_ = 10.0;        
    
    // --- 恢复更严格的装甲板匹配条件 ---
    max_angle_diff_ = 10.0; 
    max_y_diff_ratio_ = 0.5;
    // X坐标差值 / 灯条高度。真实装甲板这个比例通常在 2.0 到 4.0 之间
    max_x_diff_ratio_ = 4.5; 
}

std::vector<Armor> TraditionalDetector::detect(const cv::Mat& frame) {
    // 1. 预处理
    cv::Mat binary_img = preprocess(frame);
    // ==================== 调试代码 开始 ====================
    // 创建一个新窗口，显示预处理后的二值图像
    // 这能帮助我们判断阈值是否合适
    cv::imshow("Binary Image", binary_img);
    // ==================== 调试代码 结束 ====================
    // 2. 寻找灯条
    std::vector<cv::RotatedRect> light_bars = find_light_bars(binary_img);
    // ==================== 调试代码 开始 ====================
    // 在终端打印找到的灯条数量
    std::cout << "Found " << light_bars.size() << " light bars." << std::endl;
    // ==================== 调试代码 结束 
    // 3. 匹配装甲板
    std::vector<Armor> armors = match_armors(light_bars);
    
    return armors;
}

cv::Mat TraditionalDetector::preprocess(const cv::Mat& frame) {
    // 通道分离
    std::vector<cv::Mat> channels;
    cv::split(frame, channels);
    cv::Mat blue_channel = channels[0];
    cv::Mat red_channel = channels[2];
    
    // 通道相减 (模拟视频是蓝色装甲板)
    cv::Mat subtracted_img;
    cv::subtract(blue_channel, red_channel, subtracted_img);
    
    // 二值化
    cv::Mat binary_img;
    cv::threshold(subtracted_img, binary_img, blue_threshold_, 255, cv::THRESH_BINARY);
    
    return binary_img;
}

std::vector<cv::RotatedRect> TraditionalDetector::find_light_bars(const cv::Mat& binary_img) {
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;
    
    // 寻找轮廓
    cv::findContours(binary_img, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    
    std::vector<cv::RotatedRect> light_bars;
    for (const auto& contour : contours) {
        // 忽略面积过小的轮廓
        if (cv::contourArea(contour) < min_light_area_) {
            continue;
        }
        
        // 用旋转矩形拟合轮廓
        cv::RotatedRect rotated_rect = cv::minAreaRect(contour);
        
        // 筛选灯条 (根据长宽比)
        // 注意：OpenCV的RotatedRect的宽和高不固定，我们自己定义长和宽
        float long_side = std::max(rotated_rect.size.width, rotated_rect.size.height);
        float short_side = std::min(rotated_rect.size.width, rotated_rect.size.height);
        
        if (short_side < 1e-5) continue; // 避免除以零
        
        double aspect_ratio = long_side / short_side;
        
        if (aspect_ratio > min_light_aspect_ratio_ && aspect_ratio < max_light_aspect_ratio_) {
            light_bars.push_back(rotated_rect);
        }
    }
    return light_bars;
}

std::vector<Armor> TraditionalDetector::match_armors(const std::vector<cv::RotatedRect>& light_bars) {
    std::vector<Armor> armors;
    
    for (size_t i = 0; i < light_bars.size(); ++i) {
        for (size_t j = i + 1; j < light_bars.size(); ++j) {
            const auto& light1 = light_bars[i];
            const auto& light2 = light_bars[j];
            
            // ... (前面的筛选逻辑保持不变) ...
            float angle_diff = std::abs(light1.angle - light2.angle);
            if (angle_diff > max_angle_diff_) continue;
            
            float y_diff = std::abs(light1.center.y - light2.center.y);
            float avg_height = (light1.size.height + light2.size.height) / 2.0;
            if (avg_height < 1e-5) continue;
            if (y_diff / avg_height > max_y_diff_ratio_) continue;
            
            float x_diff = std::abs(light1.center.x - light2.center.x);
            if (x_diff / avg_height > max_x_diff_ratio_) continue;
            
            // 匹配成功
            Armor armor;
            armor.color = ArmorColor::BLUE;
            armor.confidence = 1.0;
            armor.id = "unknown";
            
            // ==================== 修正角点逻辑 开始 ====================

            cv::Point2f points1[4], points2[4];
            light1.points(points1);
            light2.points(points2);

            // 确定哪个是左灯条，哪个是右灯条
            const auto& left_light = light1.center.x < light2.center.x ? light1 : light2;
            const auto& right_light = light1.center.x < light2.center.x ? light2 : light1;
            
            cv::Point2f left_points[4], right_points[4];
            left_light.points(left_points);
            right_light.points(right_points);

            // 从左灯条的4个点里找出 "左上" 和 "左下"
            // 左边的点x坐标更小
            std::sort(left_points, left_points + 4, [](const cv::Point2f& a, const cv::Point2f& b) {
                return a.x < b.x;
            });
            cv::Point2f left_top = left_points[0].y < left_points[1].y ? left_points[0] : left_points[1];
            cv::Point2f left_bottom = left_points[0].y > left_points[1].y ? left_points[0] : left_points[1];

            // 从右灯条的4个点里找出 "右上" 和 "右下"
            // 右边的点x坐标更大
            std::sort(right_points, right_points + 4, [](const cv::Point2f& a, const cv::Point2f& b) {
                return a.x < b.x;
            });
            cv::Point2f right_top = right_points[2].y < right_points[3].y ? right_points[2] : right_points[3];
            cv::Point2f right_bottom = right_points[2].y > right_points[3].y ? right_points[2] : right_points[3];

            armor.corners.push_back(left_top);
            armor.corners.push_back(right_top);
            armor.corners.push_back(right_bottom);
            armor.corners.push_back(left_bottom);
            
            // ==================== 修正角点逻辑 结束 ====================
            
            armors.push_back(armor);
        }
    }
    return armors;
}
} // namespace rm_vision