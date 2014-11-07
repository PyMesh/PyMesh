#include "PeriodicBoundaryRemesher.h"

#include <cassert>
#include <functional>
#include <iostream>
#include <vector>

#include <Core/Exception.h>
#include <IO/MeshWriter.h>
#include <Mesh.h>
#include <MeshFactory.h>
#include <MeshUtils/Boundary.h>
#include <MeshUtils/IsolatedVertexRemoval.h>
#include <Misc/HashGrid.h>
#include <triangle/TriangleWrapper.h>

#include "BoxChecker.h"
#include "MeshCleaner.h"

namespace PeriodicBoundaryRemesherHelper {
    enum Axis { X=0, Y=1, Z=2 };
    const short min_axis_marker[3] = {-1, -2, -3};
    const short max_axis_marker[3] = { 1,  2,  3};

    void triangulate(const MatrixFr& vertices, const MatrixIr& edges,
            MatrixFr& output_vertices, MatrixIr& output_faces, Float max_area) {
        IsolatedVertexRemoval remover(vertices, edges);
        remover.run();

        TriangleWrapper triangle(remover.get_vertices(), remover.get_faces());
        triangle.run(max_area, false, true, true);

        output_vertices = triangle.get_vertices();
        output_faces = triangle.get_faces();
    }

    void save_mesh(const std::string& filename,
            const MatrixFr& vertices, const MatrixIr& faces, VectorF debug) {
        VectorF flattened_vertices(vertices.rows() * vertices.cols());
        std::copy(vertices.data(), vertices.data() + vertices.rows() *
                vertices.cols(), flattened_vertices.data());
        VectorI flattened_faces(faces.rows() * faces.cols());
        std::copy(faces.data(), faces.data() + faces.rows() * faces.cols(),
                flattened_faces.data());
        VectorI voxels = VectorI::Zero(0);

        Mesh::Ptr mesh = MeshFactory().load_data(
                flattened_vertices, flattened_faces, voxels,
                vertices.cols(), faces.cols(), 0).create_shared();
        mesh->add_attribute("debug");
        mesh->set_attribute("debug", debug);

        MeshWriter* writer = MeshWriter::create_writer(filename);
        writer->with_attribute("debug").write_mesh(*mesh);
        delete writer;
    }
}

using namespace PeriodicBoundaryRemesherHelper;

PeriodicBoundaryRemesher::PeriodicBoundaryRemesher(
        const MatrixFr& vertices, const MatrixIr& faces,
        const VectorF& bbox_min, const VectorF& bbox_max) :
    m_vertices(vertices), m_faces(faces),
    m_bbox_min(bbox_min), m_bbox_max(bbox_max) {
        assert(m_faces.cols() == 3);
    }

void PeriodicBoundaryRemesher::remesh(Float ave_edge_length) {
    clean_up();
    label_bd_faces();
    extract_bd_loops();
    match_bd_loops();
    refine_bd_loops(ave_edge_length);
    remesh_boundary(ave_edge_length * ave_edge_length);
    clean_up();
}

void PeriodicBoundaryRemesher::clean_up() {
    MeshCleaner cleaner;
    cleaner.clean(m_vertices, m_faces, 1e-3);
}

void PeriodicBoundaryRemesher::label_bd_faces() {
    m_bd_face_markers.clear();
    m_bd_face_markers.resize(m_faces.rows(), 0);

    label_bd_faces(X);
    label_bd_faces(Y);
    label_bd_faces(Z);

    VectorF labels(m_faces.rows());
    std::copy(m_bd_face_markers.begin(), m_bd_face_markers.end(), labels.data());
    save_mesh("face_labels.msh", m_vertices, m_faces, labels);
}

void PeriodicBoundaryRemesher::label_bd_faces(int axis) {
    assert(axis >=0 && axis < m_vertices.cols());
    const Float tol = 1e-3;
    const size_t num_vertices = m_vertices.rows();
    const size_t num_faces = m_faces.rows();

    std::vector<short> vertex_label(num_vertices, 0);
    for (size_t i=0; i<num_vertices; i++) {
        const VectorF& v = m_vertices.row(i);
        if (fabs(v[axis] - m_bbox_min[axis]) < tol) {
            vertex_label[i] = -1;
            m_vertices(i, axis) = m_bbox_min[axis];
        } else if (fabs(v[axis] - m_bbox_max[axis]) < tol) {
            vertex_label[i] = 1;
            m_vertices(i, axis) = m_bbox_max[axis];
        }
    }

    for (size_t i=0; i<num_faces; i++) {
        if (m_bd_face_markers[i] != 0) continue;
        const Vector3I& f = m_faces.row(i);
        Vector3I label(
                vertex_label[f[0]],
                vertex_label[f[1]],
                vertex_label[f[2]]);
        if ((label.array() > 0).all()) {
            m_bd_face_markers[i] = max_axis_marker[axis];
        } else if ((label.array() < 0).all()) {
            m_bd_face_markers[i] = min_axis_marker[axis];
        }
    }
}

void PeriodicBoundaryRemesher::extract_bd_loops() {
    m_bd_loops.clear();
    extract_bd_loops(min_axis_marker[X]);
    extract_bd_loops(max_axis_marker[X]);
    extract_bd_loops(min_axis_marker[Y]);
    extract_bd_loops(max_axis_marker[Y]);
    extract_bd_loops(min_axis_marker[Z]);
    extract_bd_loops(max_axis_marker[Z]);
}

void PeriodicBoundaryRemesher::extract_bd_loops(short label) {
    const size_t num_faces = m_faces.rows();
    std::vector<size_t> flattened_faces;
    for (size_t i=0; i<num_faces; i++) {
        if (m_bd_face_markers[i] == label) {
            const Vector3I& f = m_faces.row(i);
            flattened_faces.push_back(f[0]);
            flattened_faces.push_back(f[1]);
            flattened_faces.push_back(f[2]);
        }
    }
    assert(flattened_faces.size() > 0);

    MatrixIr faces(flattened_faces.size() / 3, 3);
    std::copy(flattened_faces.begin(), flattened_faces.end(), faces.data());

    Boundary::Ptr bd_extractor = Boundary::extract_surface_boundary_raw(
            m_vertices, faces);
    m_bd_loops.emplace(label, bd_extractor->get_boundaries());
}

void PeriodicBoundaryRemesher::match_bd_loops() {
    match_bd_loops(X);
    match_bd_loops(Y);
    match_bd_loops(Z);
}

void PeriodicBoundaryRemesher::match_bd_loops(short axis) {
    const size_t dim = m_vertices.cols();
    const size_t coord_1 = (axis+1) % dim;
    const size_t coord_2 = (axis+2) % dim;
    const size_t num_vertices = m_vertices.rows();
    VectorF offset = VectorF::Zero(dim);
    offset[axis] = m_bbox_min[axis] - m_bbox_max[axis];
    MatrixIr& min_bd_loops = m_bd_loops[min_axis_marker[axis]];
    MatrixIr& max_bd_loops = m_bd_loops[max_axis_marker[axis]];

    HashGrid::Ptr grid = HashGrid::create(1e-3, dim);

    const size_t num_min_edges = min_bd_loops.rows();
    for (size_t i=0; i<num_min_edges; i++) {
        const VectorI& e = min_bd_loops.row(i);
        const VectorF& v0 = m_vertices.row(e[0]);
        const VectorF& v1 = m_vertices.row(e[1]);

        grid->insert(e[0], v0);
        grid->insert(e[1], v1);
    }

    std::vector<int> vertex_map(num_vertices, -1);
    const size_t num_max_edges = max_bd_loops.rows();
    for (size_t i=0; i<num_max_edges; i++) {
        for (size_t j=0; j<2; j++) {
            size_t max_v_idx = max_bd_loops(i,j);
            const VectorF& max_v = m_vertices.row(max_v_idx);
            VectorF min_v = max_v + offset;
            VectorI candidates = grid->get_items_near_point(min_v);
            if (candidates.size() > 0) {
                const size_t matched_idx = candidates[0];
                vertex_map[max_v_idx] = matched_idx;
                vertex_map[matched_idx] = max_v_idx;
                m_vertices(max_v_idx, coord_1) = m_vertices(matched_idx, coord_1);
                m_vertices(max_v_idx, coord_2) = m_vertices(matched_idx, coord_2);
            }
        }
    }

    bool min_matched = true;
    size_t count = 0;
    do {
        min_matched = true;
        for (size_t i=0; i<num_min_edges; i++) {
            const VectorI& e = min_bd_loops.row(i);
            if (vertex_map[e[0]] >= 0 && vertex_map[e[1]] >= 0) {
                continue;
            } else if (vertex_map[e[0]] < 0 && vertex_map[e[1]] >= 0) {
                m_vertices.row(e[0]) = m_vertices.row(e[1]);
                vertex_map[e[0]] = vertex_map[e[1]];
            } else if (vertex_map[e[0]] >= 0 && vertex_map[e[1]] < 0) {
                m_vertices.row(e[1]) = m_vertices.row(e[0]);
                vertex_map[e[1]] = vertex_map[e[0]];
            } else {
                min_matched = false;
            }
        }
        count++;
    } while (!min_matched && count < num_min_edges);
    if (!min_matched) {
        throw RuntimeError("Unable to match min boundary loops");
    }

    bool max_matched = true;
    count = 0;
    do {
        max_matched = true;
        for (size_t i=0; i<num_max_edges; i++) {
            const VectorI& e = max_bd_loops.row(i);
            if (vertex_map[e[0]] >= 0 && vertex_map[e[1]] >= 0) {
                continue;
            } else if (vertex_map[e[0]] < 0 && vertex_map[e[1]] >= 0) {
                m_vertices.row(e[0]) = m_vertices.row(e[1]);
                vertex_map[e[0]] = vertex_map[e[1]];
            } else if (vertex_map[e[0]] >= 0 && vertex_map[e[1]] < 0) {
                m_vertices.row(e[1]) = m_vertices.row(e[0]);
                vertex_map[e[1]] = vertex_map[e[0]];
            } else {
                max_matched = false;
            }
        }
        count++;
    } while (!max_matched && count < num_max_edges);
    if (!max_matched) {
        throw RuntimeError("Unable to match max boundary loops");
    }

    // DEBUG CODE
    VectorF matched_vertices(num_vertices);
    std::copy(vertex_map.begin(), vertex_map.end(), matched_vertices.data());
    save_mesh("matched_vertices.msh", m_vertices, m_faces, matched_vertices);
}

void PeriodicBoundaryRemesher::refine_bd_loops(Float ave_edge_len) {
    // Only refine bd edges lying on the edge of the bbox.
    // i.e. These bd edges and all their adjacent faces are all on the boundary.

    const size_t num_vertices = m_vertices.rows();
    assert(m_vertices.cols() == 3);
    std::vector<Float> refined_vertices(num_vertices * 3);
    std::copy(m_vertices.data(), m_vertices.data() + num_vertices * 3,
            refined_vertices.begin());

    refine_bd_loops(min_axis_marker[X], ave_edge_len, refined_vertices);
    refine_bd_loops(max_axis_marker[X], ave_edge_len, refined_vertices);
    refine_bd_loops(min_axis_marker[Y], ave_edge_len, refined_vertices);
    refine_bd_loops(max_axis_marker[Y], ave_edge_len, refined_vertices);
    refine_bd_loops(min_axis_marker[Z], ave_edge_len, refined_vertices);
    refine_bd_loops(max_axis_marker[Z], ave_edge_len, refined_vertices);

    assert(refined_vertices.size() % 3 == 0);
    const size_t num_refined_vertices = refined_vertices.size() / 3;
    if (num_vertices != num_refined_vertices) {
        m_vertices.resize(num_refined_vertices, 3);
        std::copy(refined_vertices.begin(), refined_vertices.end(),
                m_vertices.data());
    }
}

void PeriodicBoundaryRemesher::refine_bd_loops(short label, Float ave_edge_len,
        std::vector<Float>& refined_vertices) {
    BoxChecker checker(m_bbox_min, m_bbox_max);
    MatrixIr& bd_edges = m_bd_loops[label];
    std::vector<size_t> refined_edges;

    const size_t num_edges = bd_edges.rows();
    size_t vertex_count = refined_vertices.size() / 3;
    assert(refined_vertices.size() % 3 == 0);

    for (size_t i=0; i<num_edges; i++) {
        const VectorI& e = bd_edges.row(i);
        const Vector3F& v0 = m_vertices.row(e[0]);
        const Vector3F& v1 = m_vertices.row(e[1]);
        Vector3F mid_pt = 0.5 * (v0 + v1);
        assert(checker.is_on_boundary(mid_pt));
        if (checker.is_on_boundary_edges(mid_pt)) {
            Float edge_len = (v1-v0).norm();
            size_t num_segments = size_t(std::round(edge_len / ave_edge_len));
            if (num_segments <= 1) {
                refined_edges.push_back(e[0]);
                refined_edges.push_back(e[1]);
            } else {
                refined_edges.push_back(e[0]);
                for (size_t j=1; j<num_segments; j++) {
                    Float ratio = Float(j) / Float(num_segments);
                    Vector3F p = v0 * (1.0 - ratio) + v1 * ratio;
                    refined_vertices.push_back(p[0]);
                    refined_vertices.push_back(p[1]);
                    refined_vertices.push_back(p[2]);
                    refined_edges.push_back(vertex_count);
                    refined_edges.push_back(vertex_count);
                    vertex_count++;
                }
                refined_edges.push_back(e[1]);
            }
        } else {
            refined_edges.push_back(e[0]);
            refined_edges.push_back(e[1]);
        }
    }

    assert(refined_edges.size() % 2 == 0);
    const size_t num_refined_edges = refined_edges.size() / 2;
    if (num_refined_edges != num_edges) {
        bd_edges.resize(num_refined_edges, 2);
        std::copy(refined_edges.begin(), refined_edges.end(), bd_edges.data());
    }
}

void PeriodicBoundaryRemesher::remesh_boundary(Float max_area) {
    std::vector<MatrixFr> remeshed_vertices;
    std::vector<MatrixIr> remeshed_faces;

    size_t vertex_count = 0, face_count = 0;
    add_interior_geometry(remeshed_vertices, remeshed_faces,
            vertex_count, face_count);
    triangulate_bd_loops(max_area, X, remeshed_vertices, remeshed_faces,
            vertex_count, face_count);
    triangulate_bd_loops(max_area, Y, remeshed_vertices, remeshed_faces,
            vertex_count, face_count);
    triangulate_bd_loops(max_area, Z, remeshed_vertices, remeshed_faces,
            vertex_count, face_count);

    size_t count = 0;
    m_vertices.resize(vertex_count, 3);
    for (const auto& vts : remeshed_vertices) {
        assert(count < vertex_count);
        m_vertices.block(count, 0, vts.rows(), 3) = vts;
        count += vts.rows();
    }

    count = 0;
    m_faces.resize(face_count, 3);
    for (const auto& faces : remeshed_faces) {
        assert(count < face_count);
        m_faces.block(count, 0, faces.rows(), 3) = faces;
        count += faces.rows();
    }
}

void PeriodicBoundaryRemesher::add_interior_geometry(
        std::vector<MatrixFr>& remeshed_vertices,
        std::vector<MatrixIr>& remeshed_faces,
        size_t& vertex_count,
        size_t& face_count) {

    std::vector<size_t> interior_faces;
    const size_t num_faces = m_faces.rows();
    for (size_t i=0; i<num_faces; i++) {
        if (m_bd_face_markers[i] == 0) {
            const Vector3I& f = m_faces.row(i);
            interior_faces.push_back(f[0]);
            interior_faces.push_back(f[1]);
            interior_faces.push_back(f[2]);
        }
    }

    remeshed_faces.emplace_back(MatrixIr(interior_faces.size() / 3, 3));
    std::copy(interior_faces.begin(), interior_faces.end(),
            remeshed_faces[0].data());
    remeshed_vertices.emplace_back(m_vertices);

    vertex_count += m_vertices.rows();
    face_count += interior_faces.size() / 3;
}

void PeriodicBoundaryRemesher::triangulate_bd_loops(Float max_area, short axis,
        std::vector<MatrixFr>& remeshed_vertices,
        std::vector<MatrixIr>& remeshed_faces,
        size_t& vertex_count, size_t& face_count) {
    Vector3F offset(0.0, 0.0, 0.0);
    offset[axis] = m_bbox_max[axis] - m_bbox_min[axis];
    const MatrixIr& min_loops = m_bd_loops[min_axis_marker[axis]];

    MatrixFr output_vertices;
    MatrixIr output_faces;
    triangulate(m_vertices, min_loops, output_vertices, output_faces, max_area);

    // Add triangulated min boundary
    remeshed_vertices.emplace_back(output_vertices);
    remeshed_faces.emplace_back(output_faces.array() + vertex_count);
    vertex_count += output_vertices.rows();
    face_count += output_faces.rows();

    // Add triangulated max boundary
    output_faces.col(2).swap(output_faces.col(1));
    remeshed_vertices.emplace_back(output_vertices.rowwise() + offset.transpose());
    remeshed_faces.emplace_back(output_faces.array() + vertex_count);
    vertex_count += output_vertices.rows();
    face_count += output_faces.rows();
}

