#include "BoundingBox.h"

#include <iostream>
#include <boost/function.hpp>
#include <boost/bind.hpp>

#include <TriangleMetric.h>

namespace BoundingBoxesHelper {
    void mark_as_touching(BoundingBoxes* self, const Box& a, const Box& b) {
        self->mark_as_touching(a.handle(), b.handle());
    }

    void review_touching_triangle_pairs(
            BoundingBoxes* self, const Box& a, const Box& b) {
        self->review_touching_triangle_pairs(a.handle(), b.handle());
    }

    Vector3F CGAL2Eigen(const Point_3& p) {
        return Vector3F(p.x(), p.y(), p.z());
    }

    MatrixF CGAL2Eigen(const Triangle_3& tri) {
        MatrixF M(3, 3);
        M << tri[0].x(), tri[0].y(), tri[0].z(),
             tri[1].x(), tri[1].y(), tri[1].z(),
             tri[2].x(), tri[2].y(), tri[2].z();
        return M;
    }
}

BoundingBoxes::BoundingBoxes(const MatrixF& vertices, const MatrixI& faces) : m_threshold(0.0) {
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

void BoundingBoxes::check_touching_faces(Float threshold, bool approximate) {
    clear();
    std::vector<Box> boxes = get_triangle_bboxes(threshold);
    if (approximate) {
        boost::function<void(const Box& a, const Box& b)> cb =
            boost::bind(BoundingBoxesHelper::mark_as_touching, this, _1, _2);
        CGAL::box_self_intersection_d(boxes.begin(), boxes.end(), cb);
    } else {
        m_threshold = threshold;
        boost::function<void(const Box& a, const Box& b)> cb =
            boost::bind(BoundingBoxesHelper::review_touching_triangle_pairs,
                    this, _1, _2);
        CGAL::box_self_intersection_d(boxes.begin(), boxes.end(), cb);
    }
}

void BoundingBoxes::clear() {
    m_touching_faces.resize(m_mesh.size());
    for (IndexArraies::iterator itr = m_touching_faces.begin();
            itr != m_touching_faces.end(); itr++) {
        itr->clear();
    }
}

VectorI BoundingBoxes::get_touching_face_indices(size_t id) const {
    const IndexArray& touching_indices = m_touching_faces.at(id);
    size_t num_indices = touching_indices.size();
    VectorI result(num_indices);
    std::copy(touching_indices.begin(), touching_indices.end(),
            result.data());
    return result;
}

void BoundingBoxes::review_touching_triangle_pairs(
        const TrianglesIterator& b1, const TrianglesIterator& b2) {
    using namespace BoundingBoxesHelper;
    const Triangle_3& t1 = *b1;
    const Triangle_3& t2 = *b2;
    size_t id1 = b1 - m_mesh.begin();
    size_t id2 = b2 - m_mesh.begin();

    Zhou::TriangleMetric metric;
    metric.update(CGAL2Eigen(t1), CGAL2Eigen(t2));
    Float dist = metric.getDistance();
    if (dist < m_threshold) {
        m_touching_faces[id1].push_back(id2);
        m_touching_faces[id2].push_back(id1);
    }
}

std::vector<Box> BoundingBoxes::get_triangle_bboxes(Float threshold) {
    size_t num_faces = m_mesh.size();
    std::vector<Box> boxes;
    boxes.reserve(num_faces);
    size_t count;
    for (TrianglesIterator itr=m_mesh.begin(); itr!=m_mesh.end(); itr++) {
        CGAL::Bbox_3 bbox = itr->bbox();
        CGAL::Bbox_3 expanded_bbox(
                bbox.xmin() - threshold,
                bbox.ymin() - threshold,
                bbox.zmin() - threshold,
                bbox.xmax() + threshold,
                bbox.ymax() + threshold,
                bbox.zmax() + threshold);
        boxes.push_back(Box(expanded_bbox, itr));
        count++;
    }
    return boxes;
}
