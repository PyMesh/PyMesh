/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <string>
#include <vector>
#include <memory>

#include <Core/EigenTypedef.h>

namespace PyMesh {

class GeometryCorrectionTable {
    public:
        typedef std::shared_ptr<GeometryCorrectionTable> Ptr;

        GeometryCorrectionTable(const std::string& table_file);

        void insert(const Vector2F& design, const Vector2F& measure) {
            m_design_sizes.push_back(design);
            m_measurements.push_back(measure);
        }
        void apply_correction(const Vector3F& dir, MatrixFr& loop);

    private:
        void apply_correction_to_in_plane_edge(
                const Vector3F& edge_dir, MatrixFr& loop);

        void apply_correction_to_out_plane_edge(
                const Vector3F& edge_dir, MatrixFr& loop);

        void apply_z_correction(
                const Vector3F& edge_dir, MatrixFr& loop);

        Vector2F lookup(Float half_width, Float half_height);
        Vector2F interpolate(const Vector2F& target,
                size_t idx1, size_t idx2, size_t idx3);

    private:
        std::vector<Vector2F> m_design_sizes;
        std::vector<Vector2F> m_measurements;
};

}
