/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <algorithm>
#include <string>

#include <Core/EigenTypedef.h>
#include <Boolean/BooleanEngine.h>
#include <IO/MeshWriter.h>

#include <TestBase.h>

class BooleanEngineTest : public TestBase {
    protected:
        typedef BooleanEngine::Ptr BooleanPtr;

        void save_mesh(const std::string& filename,
                const MatrixFr& vertices, const MatrixIr& faces) {
            write_mesh_raw(filename, vertices, faces);
        }

        void translate(MatrixFr& pts, const VectorF& offset) {
            const size_t num_pts = pts.rows();
            for (size_t i=0; i<num_pts; i++) {
                pts.row(i) += offset;
            }
        }
};
