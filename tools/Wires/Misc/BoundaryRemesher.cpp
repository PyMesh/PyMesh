/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "BoundaryRemesher.h"

#include <iostream>
#include <cassert>

#include <IO/MeshWriter.h>
#include <Core/Exception.h>
#include <Math/MatrixUtils.h>
#include <MeshUtils/Boundary.h>
#include <MeshUtils/SubMesh.h>
#include <MeshUtils/DuplicatedVertexRemoval.h>
#include <triangle/TriangleWrapper.h>
#include "MeshCleaner.h"

using namespace PyMesh;

namespace BoundaryRemsherHelper {
    void triangulate(MatrixFr vertices, MatrixIr edges,
            MatrixFr& output_vertices, MatrixIr& output_faces, Float max_area) {
        assert(edges.rows() >= 3);
        MeshCleaner cleaner;
        cleaner.remove_isolated_vertices(vertices, edges);
        cleaner.remove_duplicated_vertices(vertices, edges, 1e-12);
        assert(vertices.rows() >= 3);

        TriangleWrapper triangle(vertices, edges);
        triangle.run(max_area, false, true, true);

        output_vertices = triangle.get_vertices();
        output_faces = triangle.get_faces();
    }

    void reorientate_triangles(const MatrixFr& vertices, MatrixIr& faces,
            const VectorF& n) {
        assert(vertices.cols() == 3);
        assert(faces.cols() == 3);

        const VectorI& f = faces.row(0);
        const Vector3F& v0 = vertices.row(f[0]);
        const Vector3F& v1 = vertices.row(f[1]);
        const Vector3F& v2 = vertices.row(f[2]);

        Float projected_area = (v1-v0).cross(v2-v0).dot(n);
        if (projected_area < 0) {
            faces.col(2).swap(faces.col(1));
        }
    }

    void assert_faces_are_valid(const MatrixIr& faces) {
        assert((faces.col(0).array() != faces.col(1).array()).all());
        assert((faces.col(1).array() != faces.col(2).array()).all());
        assert((faces.col(2).array() != faces.col(1).array()).all());
    }

    void save_mesh(const std::string& filename,
            const MatrixFr& vertices, const MatrixIr& faces) {
        auto flattened_vertices = MatrixUtils::flatten<VectorF>(vertices);
        auto flattened_faces = MatrixUtils::flatten<VectorI>(faces);
        VectorI voxels = VectorI::Zero(0);

        MeshWriter::Ptr writer = MeshWriter::create(filename);
        writer->write(flattened_vertices, flattened_faces, voxels,
                vertices.cols(), faces.cols(), 0);
    }
}
using namespace BoundaryRemsherHelper;

BoundaryRemesher::BoundaryRemesher(const MatrixFr& vertices, const MatrixIr& faces)
    : m_vertices(vertices), m_faces(faces) {
        if (vertices.cols() != 3) {
            throw NotImplementedError(
                    "Only 3D meshes are supported for remeshing");
        }
        if (faces.cols() != 3) {
            throw NotImplementedError(
                    "Only triangle meshes are supported for remeshing");
        }
        assert_faces_are_valid(m_faces);
    }

void BoundaryRemesher::remesh(
        IndicatorFunc bd_indicator, Float max_area) {
    assert(m_vertices.rows() > 0);
    assert(m_faces.rows() > 0);

    extract_bd_faces(bd_indicator);
    remesh_bd_faces(max_area);
    merge_bd_and_non_bd_faces();
    remove_duplicated_vertices();
}

void BoundaryRemesher::extract_bd_faces(IndicatorFunc f) {
    SubMesh::Ptr submesh = SubMesh::create_raw(m_vertices, m_faces);
    submesh->filter_vertex_with_custom_function(f);
    submesh->finalize();
    m_bd_vertices = submesh->get_selected_vertices();
    m_bd_faces = submesh->get_selected_faces();
    m_non_bd_vertices = submesh->get_unselected_vertices();
    m_non_bd_faces = submesh->get_unselected_faces();

    if (m_bd_vertices.rows() == 0) {
        throw RuntimeError("Cannot remesh, zero vertex selected!");
    }
    if (m_bd_faces.rows() == 0) {
        throw RuntimeError("Cannot remesh, zero face selected!");
    }

    assert((m_bd_faces.col(0).array() != m_bd_faces.col(1).array()).all());
    assert((m_bd_faces.col(0).array() != m_bd_faces.col(2).array()).all());
    assert((m_bd_faces.col(1).array() != m_bd_faces.col(2).array()).all());
}

void BoundaryRemesher::remesh_bd_faces(Float max_area) {
    Boundary::Ptr loop_extractor = Boundary::extract_surface_boundary_raw(
            m_bd_vertices, m_bd_faces);
    auto bd_loops = loop_extractor->get_boundaries();
    triangulate(m_bd_vertices, bd_loops,
            m_bd_vertices, m_bd_faces, max_area);
}

void BoundaryRemesher::merge_bd_and_non_bd_faces() {
    const size_t num_bd_vertices = m_bd_vertices.rows();
    std::vector<MatrixFr> vertices = {m_bd_vertices, m_non_bd_vertices};
    std::vector<MatrixIr> faces = {m_bd_faces,
        m_non_bd_faces.array() + num_bd_vertices};

    m_vertices = MatrixUtils::vstack(vertices);
    m_faces = MatrixUtils::vstack(faces);
    assert_faces_are_valid(m_faces);
}

void BoundaryRemesher::remove_duplicated_vertices() {
    MeshCleaner cleaner;
    cleaner.remove_isolated_vertices(m_vertices, m_faces);
    cleaner.remove_duplicated_vertices(m_vertices, m_faces, 1e-12);
    cleaner.remove_short_edges(m_vertices, m_faces, 1e-12);
    assert_faces_are_valid(m_faces);
}
