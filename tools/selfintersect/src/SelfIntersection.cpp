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
}

SelfIntersection::SelfIntersection(
        const Eigen::MatrixXd& V,
        const Eigen::MatrixXi& F) {
    size_t num_triangles = F.rows();
    for (size_t i=0; i<num_triangles; i++) {
        m_mesh.push_back(Triangle_3(
                Point_3(V(F(i,0), 0), V(F(i,0), 1), V(F(i,0), 2)),
                Point_3(V(F(i,1), 0), V(F(i,1), 1), V(F(i,1), 2)),
                Point_3(V(F(i,2), 0), V(F(i,2), 1), V(F(i,2), 2))));
    }
}

bool SelfIntersection::detect() {
    return validate();
}

bool SelfIntersection::resolve() {
    throw NotImplementedError("Not implemented yet");
    return true;
}

bool SelfIntersection::validate() {
    m_intersection.clear();
    boost::function<void(const Box &a,const Box &b)> cb
        = boost::bind(SelfIntersectionHelper::validate_box_intersection,
                this, _1,_2);
    std::vector<Box> boxes = get_triangle_bboxes();
    CGAL::box_self_intersection_d(boxes.begin(), boxes.end(),cb);
    return m_intersection.empty();
}

Eigen::MatrixXi SelfIntersection::get_offending_triangles() {
    Eigen::MatrixXi result;
    if (m_intersection.empty()) return result;
    result.resize(m_intersection.size(), 2);

    size_t count = 0;
    for (std::vector<IndexPair>::const_iterator itr = m_intersection.begin();
            itr != m_intersection.end(); itr++) {
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
        m_intersection.push_back(IndexPair(f1_idx, f2_idx));
    }
}

std::vector<Box> SelfIntersection::get_triangle_bboxes() {
    std::vector<Box> boxes(m_mesh.size());
    for (TrianglesIterator itr = m_mesh.begin();
            itr != m_mesh.end(); itr++) {
        boxes.push_back(Box(itr->bbox(), itr));
    }
    return boxes;
}
