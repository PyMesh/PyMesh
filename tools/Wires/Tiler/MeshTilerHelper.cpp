/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "MeshTilerHelper.h"

#include <Core/EigenTypedef.h>
#include <Core/Exception.h>

#include <Wires/Misc/BilinearInterpolation.h>
#include <Wires/Misc/TrilinearInterpolation.h>

using namespace PyMesh;

TilerEngine::FuncList MeshTilerHelper::get_2D_tiling_operators(Mesh::Ptr mesh) {
    const size_t num_cells = mesh->get_num_faces();
    const size_t num_vertex_per_cell = mesh->get_vertex_per_face();
    if (num_vertex_per_cell != 4) {
        throw NotImplementedError("Only quad guide mesh is supported in 2D");
    }

    TilerEngine::FuncList operators;
    for (size_t i=0; i<num_cells; i++) {
        const VectorI face = mesh->get_face(i);
        MatrixFr corners(num_vertex_per_cell, 2);
        for (size_t j=0; j<num_vertex_per_cell; j++) {
            corners.row(j) = mesh->get_vertex(face[j]).transpose();
        }

        operators.push_back(
                [=](const MatrixFr& vertices) {
                    BilinearInterpolation interpolator(corners);
                    return interpolator.interpolate_batch(vertices);
                });
    }
    return operators;
}

TilerEngine::FuncList MeshTilerHelper::get_3D_tiling_operators(Mesh::Ptr mesh) {
    const size_t num_cells = mesh->get_num_voxels();
    const size_t num_vertex_per_cell = mesh->get_vertex_per_voxel();
    if (num_vertex_per_cell != 8) {
        throw NotImplementedError("Only hex guide mesh is supported in 3D");
    }

    TilerEngine::FuncList operators;
    for (size_t i=0; i<num_cells; i++) {
        const VectorI voxel = mesh->get_voxel(i);
        MatrixFr corners(num_vertex_per_cell, 3);
        for (size_t j=0; j<num_vertex_per_cell; j++) {
            corners.row(j) = mesh->get_vertex(voxel[j]).transpose();
        }

        operators.push_back(
                [=](const MatrixFr& vertices) {
                    TrilinearInterpolation interpolator(corners);
                    return interpolator.interpolate_batch(vertices);
                });
    }
    return operators;
}

std::vector<ParameterCommon::Variables> MeshTilerHelper::extract_face_attributes(
        Mesh::Ptr mesh) {
    const size_t num_faces = mesh->get_num_faces();
    std::vector<ParameterCommon::Variables> vars_array;
    const auto& attr_names = mesh->get_attribute_names();

    for (size_t i=0; i<num_faces; i++) {
        ParameterCommon::Variables vars;
        for (const auto& name : attr_names) {
            const MatrixFr& attr = mesh->get_attribute(name);
            if (attr.rows() != num_faces) continue;
            if (attr.cols() != 1) continue;
            vars[name] = attr(i, 0);
        }
        vars_array.push_back(vars);
    }
    return vars_array;
}

std::vector<ParameterCommon::Variables> MeshTilerHelper::extract_voxel_attributes(
        Mesh::Ptr mesh) {
    const size_t num_voxels = mesh->get_num_voxels();
    std::vector<ParameterCommon::Variables> vars_array;
    const auto& attr_names = mesh->get_attribute_names();

    for (size_t i=0; i<num_voxels; i++) {
        ParameterCommon::Variables vars;
        for (const auto& name : attr_names) {
            const MatrixFr& attr = mesh->get_attribute(name);
            if (attr.rows() != num_voxels) continue;
            if (attr.cols() != 1) continue;
            vars[name] = attr(i, 0);
        }
        vars_array.push_back(vars);
    }
    return vars_array;
}

std::vector<ParameterCommon::Variables> MeshTilerHelper::extract_attributes(
        Mesh::Ptr mesh) {
    const size_t dim = mesh->get_dim();

    if (dim == 2) {
        return extract_face_attributes(mesh);
    } else if (dim == 3) {
        return extract_voxel_attributes(mesh);
    } else {
        std::stringstream err_msg;
        err_msg << "Unsupport dim: " << dim;
        throw RuntimeError(err_msg.str());
    }
}
