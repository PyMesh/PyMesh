/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once
#include <cassert>

template<typename T>
class VectorCompare {
    public:
        bool operator()(const T& left, const T& right) const {
            const size_t dim = left.size();
            assert(dim == right.size());
            for (size_t i=0; i<dim; i++) {
                if (left[i] < right[i]) {
                    return true;
                } else if (left[i] > right[i]) {
                    return false;
                }
            }
            return false;
        }
};
