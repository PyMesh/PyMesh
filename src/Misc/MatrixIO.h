/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once
#include <cassert>
#include <string>
#include <Core/EigenTypedef.h>

#include "numpy.hpp"

namespace PyMesh {
namespace MatrixIO {
    using namespace aoba;

    template<typename Derived>
    void save_matrix(
            const std::string& filename,
            const Eigen::MatrixBase<Derived>& M) {

        int shape[] = {
            static_cast<int>(M.outerSize()),
            static_cast<int>(M.innerSize())
        };
        aoba::SaveArrayAsNumpy(filename,
                !Derived::IsRowMajor,
                2,
                shape,
                M.derived().data());
    }

    template<typename Derived>
    Derived load_matrix(const std::string& filename) {
        std::vector<int> shape;
        std::vector<typename Derived::Scalar> data;
        aoba::LoadArrayFromNumpy(filename, shape, data);
        assert(shape.size() == 2);

        Derived M(shape[0], shape[1]);
        std::copy(data.begin(), data.end(), M.data());
        return M;
    }
}
}
