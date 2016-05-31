/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "SimpleInflator.h"

#include <limits>
#include <iostream>
#include <ConvexHull/ConvexHullEngine.h>
#include <triangle/TriangleWrapper.h>

using namespace PyMesh;

namespace SimpleInflatorHelper {
    const Float EPSILON = 1e-5;

    VectorI map_indices(const VectorI& face, const VectorI& index_map) {
        const size_t vertex_per_face = face.size();
        VectorI index(vertex_per_face);
        for (size_t i=0; i<vertex_per_face; i++) {
            index[i] = index_map[face[i]];
        }
        return index;
    }

    MatrixIr generate_faces_connecting_loops(size_t loop_size, bool closed) {
        size_t num_quads;
        if (closed) {
            num_quads = loop_size;
        } else {
            num_quads = loop_size-1;
        }

        MatrixIr faces(num_quads * 2, 3);
        for (size_t i=0; i<num_quads; i++) {
            //  i       i+loop_size
            //   +-----+
            //   |    /|
            //   |   / |
            //   |  /  |
            //   | /   |
            //   |/____|
            // i+1      i+1 + loop_size

            size_t j = (i+1) % loop_size;
            faces(i*2, 0) = i;
            faces(i*2, 1) = j;
            faces(i*2, 2) = i + loop_size;
            faces(i*2+1, 0) = j;
            faces(i*2+1, 1) = j + loop_size,
            faces(i*2+1, 2) = i + loop_size;
        }
        return faces;
    }

    /**
     * Return true iff the projection of loop onto line (v0, v1) is completely
     * between v0 and v1.
     */
    bool loop_is_valid(const MatrixFr& loop,
            const VectorF& v0, const VectorF& v1) {
        VectorF dir = v1 - v0;
        Float dir_sq_len = dir.squaredNorm();
        if (dir_sq_len == 0.0) {
            throw RuntimeError("Zero edge encountered.");
        }
        VectorF proj = (loop.rowwise() - v0.transpose()) * dir / dir_sq_len;
        return ((proj.array() > 0.0).all() && (proj.array() < 1.0).all());
    }
}

using namespace SimpleInflatorHelper;

void SimpleInflator::inflate() {
    initialize();
    compute_end_loop_offsets();
    generate_end_loops();
    generate_joints();
    connect_end_loops();
    finalize();
    refine();
}

const std::vector<MatrixFr>& SimpleInflator::get_shape_velocities() const {
    throw NotImplementedError(
            "Shape velocity computation is not supported for non-periodic inflations");
}

void SimpleInflator::initialize() {
    check_thickness();
    m_end_loops.clear();
    m_vertex_list.clear();
    m_face_list.clear();
    m_face_source_list.clear();
    m_num_vertex_accumulated = 0;

    if (!m_wire_network->with_connectivity()) {
        m_wire_network->compute_connectivity();
    }
    if (!m_wire_network->has_attribute("vertex_min_angle")) {
        m_wire_network->add_attribute("vertex_min_angle");
    }
    if (!m_wire_network->has_attribute("edge_length")) {
        m_wire_network->add_attribute("edge_length");
    }
}

void SimpleInflator::compute_end_loop_offsets() {
    const size_t num_vertices = m_wire_network->get_num_vertices();
    const VectorF& min_angles =
        m_wire_network->get_attribute("vertex_min_angle");
    assert(min_angles.size() == num_vertices);
    assert(min_angles.minCoeff() > 0.0);
    VectorF vertex_thickness = compute_vertex_thickness();
    const Float max_rel_correction = m_rel_correction.maxCoeff();
    const Float max_abs_correction = m_abs_correction.maxCoeff();

    m_end_loop_offsets.resize(num_vertices);
    for (size_t i=0; i<num_vertices; i++) {
        const Float max_correction = vertex_thickness[i] * max_rel_correction
            + max_abs_correction + std::max(0.0, m_spread_const);
        m_end_loop_offsets[i] = 0.5 * (vertex_thickness[i] + sqrt(2) * max_correction) /
            tan(min_angles[i] * 0.5) + EPSILON;
    }
    validate_end_loop_offset();
}

void SimpleInflator::generate_end_loops() {
    const size_t num_edges = m_wire_network->get_num_edges();

    const MatrixFr vertices = m_wire_network->get_vertices();
    const MatrixIr edges = m_wire_network->get_edges();
    const MatrixFr edge_thickness = get_edge_thickness();
    for (size_t i=0; i<num_edges; i++) {
        const VectorI& edge = edges.row(i);
        const VectorF& v1 = vertices.row(edge[0]);
        const VectorF& v2 = vertices.row(edge[1]);
        Float edge_len = (v2 - v1).norm();
        MatrixFr loop_1 = m_profile->place(v1, v2,
                m_end_loop_offsets[edge[0]],
                edge_thickness(i, 0),
                m_rel_correction, m_abs_correction, m_correction_cap,
                m_spread_const);
        assert(loop_is_valid(loop_1, v1, v2));
        MatrixFr loop_2 = m_profile->place(v1, v2,
                edge_len - m_end_loop_offsets[edge[1]],
                edge_thickness(i, 1),
                m_rel_correction, m_abs_correction, m_correction_cap,
                m_spread_const);
        assert(loop_is_valid(loop_2, v1, v2));
        m_end_loops.push_back(std::make_pair(loop_1, loop_2));
    }
}

void SimpleInflator::generate_joints() {
    const size_t dim = m_wire_network->get_dim();
    const size_t num_vertices = m_wire_network->get_num_vertices();
    const size_t num_edges = m_wire_network->get_num_edges();
    const MatrixFr& vertices = m_wire_network->get_vertices();
    const MatrixIr& edges = m_wire_network->get_edges();
    const size_t loop_size = m_profile->size();

    std::vector<std::vector<const MatrixFr*> > loops(num_vertices);
    std::vector<std::vector<size_t> > source_edge_indices(num_vertices);
    for (size_t i=0; i<num_edges; i++) {
        const auto& end_loops = m_end_loops[i];
        const auto& edge = edges.row(i);
        loops[edge[0]].push_back(&end_loops.first);
        loops[edge[1]].push_back(&end_loops.second);
        source_edge_indices[edge[0]].push_back(i);
        source_edge_indices[edge[1]].push_back(i);
    }

    for (size_t i=0; i<num_vertices; i++) {
        const auto& incident_loops = loops[i];
        const auto& edge_ids = source_edge_indices[i];
        assert(incident_loops.size() == edge_ids.size());
        size_t valance = incident_loops.size();
        MatrixFr pts(valance * loop_size + 1, dim);
        VectorI  source_ids(pts.rows());
        pts.row(0) = vertices.row(i);
        source_ids[0] = -1;
        size_t count = 0;
        for (auto loop : incident_loops) {
            pts.block(count*loop_size+1, 0, loop_size, dim) = *loop;
            source_ids.segment(count*loop_size+1, loop_size) =
                VectorI::Ones(loop_size) * edge_ids[count];
            count++;
        }
        generate_joint(pts, source_ids, i);
    }
}

void SimpleInflator::connect_end_loops() {
    const size_t dim = m_wire_network->get_dim();
    const Float ave_thickness = m_thickness.sum() / m_thickness.size();
    const auto& edge_lengths = m_wire_network->get_attribute("edge_length");
    const size_t num_edges = m_wire_network->get_num_edges();
    const size_t loop_size = m_profile->size();

    const MatrixIr connecting_faces = generate_faces_connecting_loops(
            loop_size, dim != 2);
    const size_t num_connecting_faces = connecting_faces.rows();

    for (size_t i=0; i<num_edges; i++) {
        Float edge_length = edge_lengths(i, 0);
        const auto& end_loops = m_end_loops[i];
        const size_t num_segments = std::max(1.0,
                std::round(edge_length / ave_thickness));
        MatrixFr pts((num_segments+1)*loop_size, dim);

        for (size_t j=0; j<num_segments+1; j++) {
            Float alpha = Float(j) / Float(num_segments);
            pts.block(j*loop_size, 0, loop_size, dim) =
                end_loops.first * (1.0 - alpha) + end_loops.second * alpha;
        }

        MatrixIr faces(num_connecting_faces * num_segments, 3);
        for (size_t j=0; j<num_segments; j++) {
            faces.block(j*num_connecting_faces, 0, num_connecting_faces, 3) =
                connecting_faces.array() + j*loop_size;
        }

        m_vertex_list.push_back(pts);
        m_face_list.push_back(faces.array() + m_num_vertex_accumulated);
        m_face_source_list.push_back(int(i)*(-1)-1);
        m_num_vertex_accumulated += pts.rows();
    }
}

void SimpleInflator::finalize() {
    const size_t dim = m_wire_network->get_dim();
    size_t num_vertices = 0;
    size_t num_faces = 0;
    for (auto itr : m_vertex_list) num_vertices += itr.rows();
    for (auto itr : m_face_list) num_faces += itr.rows();

    m_vertices.resize(num_vertices, dim);
    m_faces.resize(num_faces, 3);
    m_face_sources.resize(num_faces);

    size_t vertex_count=0;
    for (auto itr : m_vertex_list) {
        size_t size = itr.rows();
        m_vertices.block(vertex_count, 0, size, dim) = itr;
        vertex_count += size;
    }

    size_t face_count=0;
    auto source_itr = m_face_source_list.begin();
    for (auto itr : m_face_list) {
        size_t size = itr.rows();
        m_faces.block(face_count, 0, size, 3) = itr;
        m_face_sources.segment(face_count, size) =
            VectorI::Ones(size) * (*source_itr);
        face_count += size;
        source_itr++;
    }

    clean_up();
}

void SimpleInflator::refine() {
    if (!m_refiner) return;
    Subdivision::Ptr subdiv = m_refiner;
    subdiv->subdivide(m_vertices, m_faces, m_subdiv_order);
    m_vertices = subdiv->get_vertices();
    m_faces = subdiv->get_faces();

    VectorI face_indices = subdiv->get_face_indices();
    const size_t num_faces = m_faces.rows();
    VectorI face_sources(num_faces);
    for (size_t i=0; i<num_faces; i++) {
        face_sources[i] = m_face_sources[face_indices[i]];
    }
    m_face_sources = face_sources;
}

VectorF SimpleInflator::compute_vertex_thickness() const {
    if (m_thickness_type == PER_VERTEX) return m_thickness;

    const size_t num_vertices = m_wire_network->get_num_vertices();
    const size_t num_edges = m_wire_network->get_num_edges();
    const MatrixIr& edges = m_wire_network->get_edges();
    VectorF vertex_thickness = VectorF::Zero(num_vertices);

    for (size_t i=0; i<num_edges; i++) {
        const VectorI& edge = edges.row(i);

        vertex_thickness[edge[0]] =
            std::max(vertex_thickness[edge[0]], m_thickness[i]);
        vertex_thickness[edge[1]] =
            std::max(vertex_thickness[edge[1]], m_thickness[i]);
    }

    assert(vertex_thickness.minCoeff() > 0.0);
    return vertex_thickness;
}

void SimpleInflator::validate_end_loop_offset() const {
    const size_t num_edges = m_wire_network->get_num_edges();
    const MatrixIr& edges = m_wire_network->get_edges();
    const auto& edge_lengths = m_wire_network->get_attribute("edge_length");

    for (size_t i=0; i<num_edges; i++) {
        const Vector2I edge = edges.row(i);
        Float edge_len = edge_lengths(i, 0);
        Float v0_offset = m_end_loop_offsets[edge[0]];
        Float v1_offset = m_end_loop_offsets[edge[1]];
        if (edge_len < v0_offset + v1_offset + EPSILON) {
            std::stringstream err_msg;
            err_msg << "Edge " << i << " is too short: len=" << edge_len <<
                std::endl;
            err_msg << "Its length needs to be  at least "
                << v0_offset + v1_offset + EPSILON
                << " to ensure inflation is self-intersection free";
            if (m_self_intersection_is_fatal) {
                throw RuntimeError(err_msg.str());
            } else {
                std::cerr << err_msg.str() << std::endl;
            }
        }
    }
}

MatrixFr SimpleInflator::get_edge_thickness() const {
    const size_t num_edges = m_wire_network->get_num_edges();
    MatrixFr thickness(num_edges, 2);
    if (m_thickness_type == PER_VERTEX) {
        const MatrixIr& edges = m_wire_network->get_edges();
        for (size_t i=0; i<num_edges; i++) {
            const VectorI& edge = edges.row(i);
            thickness(i,0) = m_thickness[edge[0]];
            thickness(i,1) = m_thickness[edge[1]];
        }
    } else if (m_thickness_type == PER_EDGE) {
        thickness.block(0,0,num_edges,1) = m_thickness;
        thickness.block(0,1,num_edges,1) = m_thickness;
    }
    return thickness;
}

void SimpleInflator::generate_joint(
        const MatrixFr& pts, const VectorI& source_ids, size_t vertex_index) {
    const size_t dim = m_wire_network->get_dim();
    ConvexHullEngine::Ptr convex_hull = ConvexHullEngine::create(dim, "qhull");
    convex_hull->run(pts);

    MatrixFr vertices = convex_hull->get_vertices();
    MatrixIr faces = convex_hull->get_faces();
    VectorI  index_map = convex_hull->get_index_map();

    if (dim == 2) {
        // Need to triangulate the loop.
        const size_t num_vertices = vertices.rows();
        TriangleWrapper tri(vertices, faces);
        tri.run(std::numeric_limits<Float>::max(), false, false, false);
        vertices = tri.get_vertices();
        faces = tri.get_faces();
        assert(vertices.rows() == num_vertices);
    }

    m_vertex_list.push_back(vertices);
    const size_t num_faces = faces.rows();
    for (size_t i=0; i<num_faces; i++) {
        const auto& face = faces.row(i);
        if (dim == 3) {
            auto ori_indices = map_indices(face, index_map);
            if (belong_to_the_same_loop(ori_indices, source_ids)) continue;
        }
        m_face_list.push_back(face.array() + m_num_vertex_accumulated);
        m_face_source_list.push_back(vertex_index+1);
    }

    m_num_vertex_accumulated += vertices.rows();
}

bool SimpleInflator::belong_to_the_same_loop(
        const VectorI& indices, const VectorI& source_ids) const {
    const size_t size = indices.size();
    assert(size > 0);
    const int id = source_ids[indices[0]];
    for (size_t i=1; i<size; i++) {
        if (id != source_ids[indices[i]]) return false;
    }
    return true;
}
