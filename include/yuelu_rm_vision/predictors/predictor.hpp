#pragma once

#include "common/types.hpp"

namespace rm_vision {

class Predictor {
public:
    // 纯虚函数，定义预测器的核心功能
    // 输入当前的目标状态，返回预测后的目标状态
    virtual Target predict(const Target& current_target) = 0;

    virtual ~Predictor() = default;
};

} // namespace rm_vision