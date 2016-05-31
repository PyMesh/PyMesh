/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "SelfIntersection.h"

#include <Core/Exception.h>
#include <Math/MatrixUtils.h>

#include <boost/function.hpp>
#include <boost/bind.hpp>

#include <CGAL/box_intersection_d.h>

using namespace PyMesh;

namespace SelfIntersectionHelper {
    class Box : public CGAL::Box_intersection_d::Box_d<Float,
          3, CGAL::Box_intersection_d::ID_NONE> {
        public:
            typedef CGAL::Box_intersection_d::Box_d<Float, 3,
                    CGAL::Box_intersection_d::ID_NONE> Base;
            typedef Float                   NT;
            typedef std::size_t             ID;

            Box() {}
            Box(bool complete) : Base(complete) {}
            Box(NT l[3], NT h[3]) : Base( l, h) {}
            Box( const CGAL::Bbox_3& b) : Base( b) {}
            ID  id() const { return m_id; }
            void set_id(ID id) { m_id = id; }
        private:
            ID m_id;
    };

    void handle_intersection_candidate(
            SelfIntersection* self, const Box& a, const Box& b) {
        self->handle_intersection_candidate(a.id(), b.id());
    }

    Vector2I get_opposite_edge(const Vector3I& f, size_t v) {
        if (f[0] == v) {
            return Vector2I(f[1], f[2]);
        } else if (f[1] == v) {
            return Vector2I(f[2], f[0]);
        } else if (f[2] == v) {
            return Vector2I(f[0], f[1]);
        } else {
            std::stringstream err_msg;
            err_msg << "Vertex " << v << " does not belong to triangle ("
                << f.transpose() << ")";
            throw RuntimeError(err_msg.str());
        }
    }

    size_t get_opposite_vertex(const Vector3I& f, const Vector2I& e) {
        if (f[0] != e[0] && f[0] != e[1]) return f[0];
        if (f[1] != e[0] && f[1] != e[1]) return f[1];
        if (f[2] != e[0] && f[2] != e[1]) return f[2];
        throw RuntimeError("Face must be topologically degnerated!");
    }

    std::vector<Box> get_triangle_bboxes(
            const SelfIntersection::Points& pts, const MatrixIr& faces) {
        const size_t num_faces = faces.rows();
        std::vector<Box> boxes;
        boxes.reserve(num_faces);
        for (size_t i=0; i<num_faces; i++) {
            const Vector3I f = faces.row(i);
            const std::vector<SelfIntersection::Point_3> corners{
                pts[f[0]], pts[f[1]], pts[f[2]]
            };
            if (CGAL::collinear(pts[f[0]], pts[f[1]], pts[f[2]])) {
                // Triangle is degenerated.
                continue;
            }
            boxes.emplace_back(CGAL::bbox_3(corners.begin(), corners.end()));
            boxes.back().set_id(i);
        }
        return boxes;
    }
}
using namespace SelfIntersectionHelper;

SelfIntersection::SelfIntersection(
        const MatrixFr& vertices, const MatrixIr& faces)
: m_faces(faces) {
    const size_t num_vertices = vertices.rows();
    const size_t dim = vertices.cols();
    const size_t num_faces = faces.rows();
    const size_t vertex_per_face = faces.cols();

    if (dim != 3) {
        throw NotImplementedError(
                "Self intersection check only support 3D");
    }
    if (vertex_per_face != 3) {
        throw NotImplementedError(
                "Self intersection check only works with triangles");
    }

    m_points.resize(num_vertices);
    for (size_t i=0; i<num_vertices; i++) {
        m_points[i] = Point_3(
                vertices(i,0),
                vertices(i,1),
                vertices(i,2));
    }
}

void SelfIntersection::detect_self_intersection() {
    clear();
    std::vector<Box> boxes = get_triangle_bboxes(m_points, m_faces);
    boost::function<void(const Box& a, const Box& b)> cb =
        boost::bind(SelfIntersectionHelper::handle_intersection_candidate,
                this, _1, _2);
    CGAL::box_self_intersection_d(boxes.begin(), boxes.end(), cb);
}

void SelfIntersection::clear() {
    m_intersecting_pairs.clear();
}

void SelfIntersection::handle_intersection_candidate(
        size_t f_idx_1, size_t f_idx_2) {
    auto duplicated_vertices = topological_overlap(f_idx_1, f_idx_2);
    const Vector3I f1 = m_faces.row(f_idx_1);
    const Vector3I f2 = m_faces.row(f_idx_2);
    const Triangle_3 t1(m_points[f1[0]], m_points[f1[1]], m_points[f1[2]]);
    const Triangle_3 t2(m_points[f2[0]], m_points[f2[1]], m_points[f2[2]]);

    bool is_intersecting = false;
    const size_t num_duplicated_vertices = duplicated_vertices.size();
    switch (num_duplicated_vertices) {
        case 0:
            // triangles do not touch.
            {
                bool t1_degenerate = t1.is_degenerate();
                bool t2_degenerate = t2.is_degenerate();
                if (t1_degenerate || t2_degenerate) {
                    // Degenerated triangles are considered as
                    // self-intersecting.
                    is_intersecting = true;
                } else {
                    is_intersecting = CGAL::do_intersect(t1, t2);
                }
            }
            break;
        case 3:
            // duplicated face
            is_intersecting = true;
            break;
        case 1:
            {
                // touch at a vertex.
                size_t shared_vertex = duplicated_vertices[0];
                Vector2I opp_edge_1 = get_opposite_edge(f1, shared_vertex);
                Vector2I opp_edge_2 = get_opposite_edge(f2, shared_vertex);
                Segment_3 seg_1(m_points[opp_edge_1[0]], m_points[opp_edge_1[1]]);
                Segment_3 seg_2(m_points[opp_edge_2[0]], m_points[opp_edge_2[1]]);
                is_intersecting =
                    CGAL::do_intersect(t1, seg_2) ||
                    CGAL::do_intersect(t2, seg_1);
            }
            break;
        case 2:
            {
                // touch at an edge.
                Vector2I shared_edge(duplicated_vertices[0],
                        duplicated_vertices[1]);
                size_t v1 = get_opposite_vertex(f1, shared_edge);
                size_t v2 = get_opposite_vertex(f2, shared_edge);
                const auto& p1 = m_points[v1];
                const auto& p2 = m_points[v2];
                const auto& p3 = m_points[shared_edge[0]];
                const auto& p4 = m_points[shared_edge[1]];
                if (CGAL::coplanar(p1, p2, p3, p4)) {
                    if (CGAL::collinear(p3, p4, p1)) {
                        is_intersecting = true;
                    } else if (CGAL::collinear(p3, p4, p2)) {
                        is_intersecting = true;
                    } else {
                        switch (CGAL::coplanar_orientation(p3, p4, p1, p2)) {
                            case CGAL::POSITIVE:
                                is_intersecting = true;
                                break;
                            case CGAL::NEGATIVE:
                                is_intersecting = false;
                                break;
                            case CGAL::COLLINEAR:
                                throw RuntimeError(
                                        "Inconsistent CGAL predicate output");
                                break;
                        }
                    }
                } else {
                    is_intersecting = false;
                }
            }
            break;
        default:
            throw RuntimeError(
                    "Two triangles sharing more than 3 vertices? Something is very wrong");
    }

    if (is_intersecting) {
        m_intersecting_pairs.emplace_back(f_idx_1, f_idx_2);
    }
}

std::vector<size_t> SelfIntersection::topological_overlap(size_t id1, size_t id2) const {
    Vector3I f1 = m_faces.row(id1);
    Vector3I f2 = m_faces.row(id2);
    std::vector<size_t> duplicated_vertices;
    duplicated_vertices.reserve(3);
    for (size_t i=0; i<3; i++) {
        for (size_t j=0; j<3; j++) {
            if (f1[i] == f2[j]) {
                duplicated_vertices.push_back(f1[i]);
            }
        }
    }
    return duplicated_vertices;
}
