/* This file is part of PyMesh. Copyright (c) 2016 by Qingnan Zhou */
#pragma once

#include <string>
#include <igl/readDMAT.h>
#include <igl/writeDMAT.h>

namespace PyMesh {
namespace DMAT {
    template<typename Derived>
    Derived load_matrix(const std::string& filename) {
        Derived r;
        igl::readDMAT(filename, r);
        return r;
    }

    template<typename Derived>
    void save_matrix(const std::string& filename,
            const Eigen::MatrixBase<Derived>& M,
            bool in_ascii=false) {
        igl::writeDMAT(filename, M, in_ascii);
    }
}

}
