#pragma once

#include "yuelu_rm_vision/solvers/solver.hpp"

namespace rm_vision {

class PnPSolver : public Solver {
public:
    // 构造函数，加载相机参数和装甲板3D模型
    PnPSolver(const cv::Mat& camera_matrix, const cv::Mat& dist_coeffs);
    
    // 实现基类的 solve 接口
    std::vector<Target> solve(const std::vector<Armor>& armors) override;

private:
    // 相机内参
    cv::Mat camera_matrix_;
    cv::Mat dist_coeffs_;

    // 装甲板3D模型点 (单位: mm)
    // 我们定义一个"小装甲板"和一个"大装甲板"的模型
    std::vector<cv::Point3f> small_armor_points_;
    std::vector<cv::Point3f> large_armor_points_;
};

} // namespace rm_vision