#include "SelfIntersection.h"
#include "Exception.h"

#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <cassert>
#include <iostream>

#include "TriangleIntersection.h"

namespace SelfIntersectionHelper {
    void validate_box_intersection(
            SelfIntersection* obj,
            const Box& box_1, 
            const Box& box_2) {
        obj->validate_box_intersection(box_1, box_2);
    }

    void resolve_box_intersection(
            SelfIntersection* obj,
            const Box& box_1, 
            const Box& box_2) {
        obj->resolve_box_intersection(box_1, box_2);
    }
}

SelfIntersection::SelfIntersection(
        const Eigen::MatrixXd& V,
        const Eigen::MatrixXi& F) {
    size_t num_vertices = V.rows();
    size_t num_triangles = F.rows();

    m_vertices.resize(num_vertices);
    for (size_t i=0; i<num_vertices; i++) {
        m_vertices[i] = Point_3(V(i, 0), V(i, 1), V(i, 2));
    }

    m_faces.resize(num_triangles);
    for (size_t i=0; i<num_triangles; i++) {
        m_mesh.push_back(Triangle_3(
                    m_vertices[F(i, 0)],
                    m_vertices[F(i, 1)],
                    m_vertices[F(i, 2)]));
        m_faces[i][0] = F(i, 0);
        m_faces[i][1] = F(i, 1);
        m_faces[i][2] = F(i, 2);
    }
}

bool SelfIntersection::detect() {
    return validate();
}

bool SelfIntersection::resolve() {
    m_edge_map.clear();
    m_intersecting_triangle_pairs.clear();
    m_intersection_objects.clear();
    m_intersection_objects.resize(m_mesh.size());
    boost::function<void(const Box &a,const Box &b)> cb
        = boost::bind(SelfIntersectionHelper::resolve_box_intersection,
                this, _1,_2);
    std::vector<Box> boxes = get_triangle_bboxes();
    CGAL::box_self_intersection_d(boxes.begin(), boxes.end(),cb);

    initialize_resolved_mesh();
    copy_unoffending_triangles();
    triangulate_offending_triangles();
}

bool SelfIntersection::validate() {
    m_intersecting_triangle_pairs.clear();
    boost::function<void(const Box &a,const Box &b)> cb
        = boost::bind(SelfIntersectionHelper::validate_box_intersection,
                this, _1,_2);
    std::vector<Box> boxes = get_triangle_bboxes();
    CGAL::box_self_intersection_d(boxes.begin(), boxes.end(),cb);
    return m_intersecting_triangle_pairs.empty();
}

Eigen::MatrixXi SelfIntersection::get_offending_triangles() {
    Eigen::MatrixXi result;
    if (m_intersecting_triangle_pairs.empty()) return result;
    result.resize(m_intersecting_triangle_pairs.size(), 2);

    size_t count = 0;
    for (std::vector<IndexPair>::const_iterator
            itr = m_intersecting_triangle_pairs.begin();
            itr != m_intersecting_triangle_pairs.end(); itr++) {
        result(count, 0) = itr->first;
        result(count, 1) = itr->second;
        count++;
    }

    return result;
}

void SelfIntersection::validate_box_intersection(
        const Box& box_1, const Box& box_2) {
    int f1_idx = box_1.handle()-m_mesh.begin();
    int f2_idx = box_2.handle()-m_mesh.begin();
    const Triangle_3 & tri1 = *box_1.handle();
    const Triangle_3 & tri2 = *box_2.handle();

    if (TriangleIntersection::detect(tri1, tri2, false)) {
        m_intersecting_triangle_pairs.push_back(IndexPair(f1_idx, f2_idx));
    }
}

void SelfIntersection::resolve_box_intersection(
        const Box& box_1, const Box& box_2) {
    int f1_idx = box_1.handle()-m_mesh.begin();
    int f2_idx = box_2.handle()-m_mesh.begin();
    const Triangle_3 & tri1 = *box_1.handle();
    const Triangle_3 & tri2 = *box_2.handle();

    CGAL::Object intersection =
        TriangleIntersection::resolve(tri1, tri2, false);

    if (intersection.empty()) { return; }

    m_intersecting_triangle_pairs.push_back(IndexPair(f1_idx, f2_idx));
    m_intersection_objects[f1_idx].push_back(intersection);
    m_intersection_objects[f2_idx].push_back(intersection);
}

std::vector<Box> SelfIntersection::get_triangle_bboxes() {
    std::vector<Box> boxes;
    boxes.reserve(m_mesh.size());
    for (TrianglesIterator itr = m_mesh.begin();
            itr != m_mesh.end(); itr++) {
        boxes.push_back(Box(itr->bbox(), itr));
    }
    return boxes;
}

void SelfIntersection::initialize_resolved_mesh() {
    // All vertices are carried over.
    m_resolved_vertices = m_vertices;

    m_resolved_mesh.clear();
    m_resolved_faces.clear();
}

void SelfIntersection::copy_unoffending_triangles() {
    typedef SelfIntersection::IntersectionObjects IntersectionObj;
    typedef std::vector<IntersectionObj> Intersections;
    for (size_t i=0; i<m_mesh.size(); i++) {
        if (m_intersection_objects.at(i).empty()) {
            m_resolved_mesh.push_back(m_mesh[i]);
            m_resolved_faces.push_back(m_faces[i]);
        }
    }
}

void SelfIntersection::triangulate_offending_triangles() {
    typedef SelfIntersection::IntersectionObjects IntersectionObj;
    typedef std::vector<IntersectionObj> Intersections;
    for (size_t i=0; i<m_mesh.size(); i++) {
        if (!m_intersection_objects.at(i).empty()) {
        }
    }
}

