#pragma once

#include <vector>

namespace MatrixUtils {
    template<typename T>
    T vstack(const std::vector<T>& matrices);
}

#include "MatrixUtils.inl"
