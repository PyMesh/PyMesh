/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "GeometryCorrectionTable.h"
#include <algorithm>
#include <cstdio>
#include <cassert>
#include <cmath>
#include <iostream>

#include <fstream>
#include <Core/Exception.h>

using namespace PyMesh;

namespace GeometryCorrectionTableHelper {
    const size_t num_offset_pixel = 0;
    const Float min_thickness = 0.1;
    Float signed_area(
            const Vector2F& v1,
            const Vector2F& v2,
            const Vector2F& v3) {
        return 0.5 * (
                (v2[0] - v1[0]) * (v3[1] - v1[1]) -
                (v2[1] - v1[1]) * (v3[0] - v1[0]));
    }
}
using namespace GeometryCorrectionTableHelper;

GeometryCorrectionTable::GeometryCorrectionTable(const std::string& table_file) {
    std::ifstream fin(table_file.c_str());

    const size_t LINE_SIZE = 256;
    char line[LINE_SIZE];

    size_t count = 0;
    while (!fin.eof()) {
        fin.getline(line, LINE_SIZE);
        if (count == 0) {
            count++;
            continue;
        }

        Float data[4];
        size_t n = sscanf(line, "%lf, %lf, %lf, %lf",
                &data[0], &data[1], &data[2], &data[3]);

        Vector2F design_size(data[0], data[1]);
        Vector2F measured_size(data[2], data[3]);
        m_design_sizes.push_back(design_size);
        m_measurements.push_back(measured_size);
    }
    fin.close();
}

void GeometryCorrectionTable::apply_correction(const Vector3F& dir, MatrixFr& loop) {
    Float edge_len = dir.norm();
    assert(edge_len > 1e-12);
    Vector3F edge_dir = dir.normalized();
    if (fabs(edge_dir[2]) < 1e-3) {
        apply_correction_to_in_plane_edge(edge_dir, loop);
    } else {
        apply_correction_to_out_plane_edge(edge_dir, loop);
    }
    apply_z_correction(edge_dir, loop);
}

void GeometryCorrectionTable::apply_correction_to_in_plane_edge(
        const Vector3F& edge_dir, MatrixFr& loop) {
    VectorF bbox_min = loop.colwise().minCoeff();
    VectorF bbox_max = loop.colwise().maxCoeff();
    VectorF bbox_center = 0.5 * (bbox_min + bbox_max);

    size_t num_vts = loop.rows();
    size_t dim = loop.cols();
    assert(dim == 3);
    MatrixFr proj_loop(num_vts, dim);

    for (size_t i=0; i<num_vts; i++) {
        const VectorF& v = loop.row(i) - bbox_center.transpose();
        proj_loop.row(i) = Vector3F(v[0], v[1], 0.0);
    }
    Float target_half_height = 1e3; // Something huge to represent inf
    Float target_half_width = proj_loop.row(0).norm();

    Vector2F correction_1 = lookup(target_half_width, target_half_height);
    Vector2F correction_2 = lookup(target_half_height, target_half_width);
    Float half_width  = 0.5 * (correction_1[0] + correction_2[1])
        + 0.05 * num_offset_pixel;
    half_width = std::max(half_width, min_thickness);

    for (size_t i=0; i<num_vts; i++) {
        loop.row(i) += proj_loop.row(i) *
            (-target_half_width + half_width) / target_half_width;
    }
}

void GeometryCorrectionTable::apply_correction_to_out_plane_edge(
        const Vector3F& edge_dir, MatrixFr& loop) {
    const Float EPS = 1e-3;
    assert(fabs(edge_dir[2]) > 0.0);
    VectorF bbox_min = loop.colwise().minCoeff();
    VectorF bbox_max = loop.colwise().maxCoeff();
    VectorF bbox_center = 0.5 * (bbox_min + bbox_max);

    size_t num_vts = loop.rows();
    size_t dim = loop.cols();
    assert(dim == 3);
    MatrixFr proj_loop(num_vts, 3);
    Vector3F proj_edge_dir(edge_dir[0], edge_dir[1], 0.0);

    if (loop.rows() != 4) {
        throw NotImplementedError(
                "Geometry correction supports only square wires");
    }

    for (size_t i=0; i<num_vts; i++) {
        VectorF v = loop.row(i) - bbox_center.transpose();
        Float edge_dir_offset = v[2] / edge_dir[2];
        VectorF proj_v = v - edge_dir * edge_dir_offset;
        proj_loop.row(i) = proj_v;
        assert(fabs(proj_v[2]) < EPS);
    }

    Float dist_01 = (proj_loop.row(0) - proj_loop.row(1)).norm();
    Float dist_12 = (proj_loop.row(1) - proj_loop.row(2)).norm();
    if (dist_01 > dist_12) {
        proj_edge_dir = (proj_loop.row(0) - proj_loop.row(1)) / dist_01;
    } else {
        proj_edge_dir = (proj_loop.row(1) - proj_loop.row(2)) / dist_12;
    }

    const VectorF& corner = proj_loop.row(0);
    Float target_half_height = proj_edge_dir.dot(corner);
    Float target_half_width =
        (corner - proj_edge_dir * target_half_height).norm();
    target_half_height = fabs(target_half_height);

    Vector2F correction_1 = lookup(target_half_width, target_half_height);
    Vector2F correction_2 = lookup(target_half_height, target_half_width);
    Float half_width  = 0.5 * (correction_1[0] + correction_2[1])
        + 0.05 * num_offset_pixel;
    Float half_height = 0.5 * (correction_1[1] + correction_2[0])
        + 0.05 * num_offset_pixel;
    half_width = std::max(half_width, min_thickness);
    half_height = std::max(half_height, min_thickness);

    for (size_t i=0; i<num_vts; i++) {
        const VectorF& proj_v = proj_loop.row(i);
        Float height = proj_edge_dir.dot(proj_v);
        VectorF width_dir = (proj_v - proj_edge_dir * height).normalized();
        assert(!isnan(width_dir[0]));
        assert(!isnan(width_dir[1]));
        assert(!isnan(width_dir[2]));

        Float height_sign = (height < 0.0)? -1: 1;
        proj_loop.row(i) = proj_edge_dir * height_sign * half_height
            + width_dir * half_width;
    }

    for (size_t i=0; i<num_vts; i++) {
        const VectorF& proj_v = proj_loop.row(i);
        loop.row(i) = (bbox_center + proj_v - edge_dir *
                edge_dir.dot(proj_v)).transpose();
    }
}

void GeometryCorrectionTable::apply_z_correction(
        const Vector3F& edge_dir, MatrixFr& loop) {
    //const Float max_z_error = 0.125;
    //const Float max_z_error = 0.09;
    const Float max_z_error = 0.00;
    VectorF bbox_min = loop.colwise().minCoeff();
    VectorF bbox_max = loop.colwise().maxCoeff();
    VectorF bbox_center = 0.5 * (bbox_min + bbox_max);

    Vector3F side_dir = edge_dir.cross(Vector3F::UnitZ());
    Float sin_val = side_dir.norm();
    if (sin_val < 1e-3) return;

    const size_t num_vts = loop.rows();
    for (size_t i=0; i<num_vts; i++) {
        Vector3F v = loop.row(i) - bbox_center.transpose();
        Float side_component = side_dir.dot(v) / sin_val;
        Vector3F proj_v = v - side_component * side_dir / sin_val;
        Float proj_component = proj_v.norm();
        if (proj_component > 1e-3) {
            proj_v -= proj_v / proj_component * (sin_val * max_z_error);
        }
        loop.row(i) = bbox_center + proj_v + side_component * side_dir / sin_val;
    }
}

Vector2F GeometryCorrectionTable::lookup(Float half_width, Float half_height) {
    const size_t num_entries = m_measurements.size();
    assert(m_design_sizes.size() == num_entries);
    Vector2F target(half_width*2, half_height*2);

    std::vector<Float> dist(num_entries);
    std::vector<size_t> indices(num_entries);
    for (size_t i=0; i<num_entries; i++) {
        const Vector2F& measure = m_measurements[i];
        dist[i] = (target - measure).squaredNorm();
        indices[i] = i;
    }

    auto index_compare = [=](size_t i, size_t j) { return dist[i] < dist[j]; };
    std::sort(indices.begin(), indices.end(), index_compare);

    return interpolate(target, indices[0], indices[1], indices[2]) * 0.5;
}

Vector2F GeometryCorrectionTable::interpolate(const Vector2F& target,
        size_t idx1, size_t idx2, size_t idx3) {
    const Vector2F& measure1 = m_measurements[idx1];
    const Vector2F& measure2 = m_measurements[idx2];
    const Vector2F& measure3 = m_measurements[idx3];
    Float area = signed_area(measure1, measure2, measure3);
    Float area1 = signed_area(target, measure2, measure3);
    Float area2 = signed_area(target, measure3, measure1);
    Float area3 = signed_area(target, measure1, measure2);
    Float b1 = area1 / area;
    Float b2 = area2 / area;
    Float b3 = area3 / area;

    if (b1 < 0.0 || b2 < 0.0 || b3 < 0.0) {
        // Extrapolate
        // Use the error of the closest measurement.
        return target - m_measurements[idx1] + m_design_sizes[idx1];
    } else {
        // Interpolate
        const Vector2F design1 = m_design_sizes[idx1];
        const Vector2F design2 = m_design_sizes[idx2];
        const Vector2F design3 = m_design_sizes[idx3];
        return design1 * b1 + design2 * b2 + design3 * b3;
    }
}

