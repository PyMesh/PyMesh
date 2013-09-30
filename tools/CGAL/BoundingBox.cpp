#include "BoundingBox.h"

#include <iostream>
#include <boost/function.hpp>
#include <boost/bind.hpp>

namespace BoundingBoxesHelper {
    void mark_as_overlap(BoundingBoxes* self, const Box& a, const Box& b) {
        self->mark_as_overlap(a.handle(), b.handle());
    }
}

BoundingBoxes::BoundingBoxes(const MatrixF& vertices, const MatrixI& faces) {
    size_t num_vertices = vertices.rows();
    size_t num_faces = faces.rows();

    std::vector<Point_3> pts(num_vertices);
    for (size_t i=0; i<num_vertices; i++) {
        pts[i] = Point_3(
                vertices(i,0),
                vertices(i,1),
                vertices(i,2));
    }

    m_mesh.clear();
    for (size_t i=0; i<num_faces; i++) {
        Triangle_3 tri(
                pts[faces(i, 0)],
                pts[faces(i, 1)],
                pts[faces(i, 2)]);
        m_mesh.push_back(tri);
    }
}

void BoundingBoxes::check_overlap() {
    clear();
    std::vector<Box> boxes = get_triangle_bboxes();
    boost::function<void(const Box& a, const Box& b)> cb =
        boost::bind(BoundingBoxesHelper::mark_as_overlap, this, _1, _2);
    CGAL::box_self_intersection_d(boxes.begin(), boxes.end(), cb);
}

void BoundingBoxes::clear() {
    m_overlap_array.resize(m_mesh.size());
    for (IndexArraies::iterator itr = m_overlap_array.begin();
            itr != m_overlap_array.end(); itr++) {
        itr->clear();
    }
}

VectorI BoundingBoxes::get_overlapping_obj_indices(size_t id) const {
    const IndexArray& overlapping_indices = m_overlap_array.at(id);
    size_t num_indices = overlapping_indices.size();
    VectorI result(num_indices);
    std::copy(overlapping_indices.begin(), overlapping_indices.end(),
            result.data());
    return result;
}

std::vector<Box> BoundingBoxes::get_triangle_bboxes() {
    size_t num_faces = m_mesh.size();
    std::vector<Box> boxes;
    boxes.reserve(num_faces);
    size_t count;
    for (TrianglesIterator itr=m_mesh.begin(); itr!=m_mesh.end(); itr++) {
        boxes.push_back(Box(itr->bbox(), itr));
        count++;
    }
    return boxes;
}
