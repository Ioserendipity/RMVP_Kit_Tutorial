#include "yuelu_rm_vision/solvers/pnp_solver.hpp"
#include <vector>

namespace rm_vision {

PnPSolver::PnPSolver(const cv::Mat& camera_matrix, const cv::Mat& dist_coeffs)
    : camera_matrix_(camera_matrix), dist_coeffs_(dist_coeffs) {
    
    // 根据RoboMaster规则手册定义3D模型点
    // 注意：坐标系原点在装甲板中心
    // 小装甲板物理尺寸：130mm x 55mm
    float small_half_w = 130.0f / 2.0f;
    float small_half_h = 55.0f / 2.0f;
    small_armor_points_.emplace_back(-small_half_w,  small_half_h, 0); // top-left
    small_armor_points_.emplace_back( small_half_w,  small_half_h, 0); // top-right
    small_armor_points_.emplace_back( small_half_w, -small_half_h, 0); // bottom-right
    small_armor_points_.emplace_back(-small_half_w, -small_half_h, 0); // bottom-left

    // 大装甲板物理尺寸：230mm x 55mm
    float large_half_w = 230.0f / 2.0f;
    float large_half_h = 55.0f / 2.0f;
    large_armor_points_.emplace_back(-large_half_w,  large_half_h, 0);
    large_armor_points_.emplace_back( large_half_w,  large_half_h, 0);
    large_armor_points_.emplace_back( large_half_w, -large_half_h, 0);
    large_armor_points_.emplace_back(-large_half_w, -large_half_h, 0);
}

std::vector<Target> PnPSolver::solve(const std::vector<Armor>& armors) {
    std::vector<Target> targets;
    
    for (const auto& armor : armors) {
        // 根据装甲板ID选择3D模型
        // 临时逻辑：假设ID "1" 是大装甲板，其他是小装甲板
        const auto& object_points = (armor.id == "1") ? large_armor_points_ : small_armor_points_;
        
        // PnP需要至少4个点
        if (armor.corners.size() != 4) {
            continue;
        }

        cv::Mat rvec, tvec; // 旋转向量 和 平移向量
        
        // 调用OpenCV的solvePnP函数
        bool success = cv::solvePnP(
            object_points,     // 3D模型点
            armor.corners,       // 2D图像点
            camera_matrix_,      // 相机内参
            dist_coeffs_,        // 畸变系数
            rvec,                // 输出的旋转向量
            tvec,                // 输出的平移向量
            false,               // 不使用外参的初始猜测
            cv::SOLVEPNP_IPPE    // 使用一个快速且稳定的算法
        );
        
        if (success) {
            Target target;
            target.id = armor.id;
            target.position = cv::Point3f(tvec); // tvec就是相机坐标系下的3D位置
            // 速度暂时为0
            target.velocity = cv::Point3f(0, 0, 0);
            
            targets.push_back(target);
        }
    }
    return targets;
}

} // namespace rm_vision