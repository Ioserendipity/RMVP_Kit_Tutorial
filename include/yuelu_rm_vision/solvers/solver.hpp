#pragma once

#include "common/types.hpp"
#include <vector>

namespace rm_vision {

class Solver {
public:
    // 纯虚函数，定义姿态解算器的核心功能
    // 输入检测到的装甲板列表，返回解算出的3D目标列表
    virtual std::vector<Target> solve(const std::vector<Armor>& armors) = 0;

    virtual ~Solver() = default;
};

} // namespace rm_vision