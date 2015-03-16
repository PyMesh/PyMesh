#include "SelfIntersection.h"

#include <Math/MatrixUtils.h>

#include <boost/function.hpp>
#include <boost/bind.hpp>

namespace SelfIntersectionHelper {
    void handle_intersection_candidate(
            SelfIntersection* self, const Box& a, const Box& b) {
        self->handle_intersection_candidate(a.handle(), b.handle());
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
using namespace SelfIntersectionHelper;

SelfIntersection::SelfIntersection(
        const MatrixFr& vertices, const MatrixIr& faces)
: m_vertices(vertices), m_faces(faces) {
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

void SelfIntersection::detect_self_intersection() {
    clear();
    std::vector<Box> boxes = get_triangle_bboxes();
    boost::function<void(const Box& a, const Box& b)> cb =
        boost::bind(SelfIntersectionHelper::handle_intersection_candidate,
                this, _1, _2);
    CGAL::box_self_intersection_d(boxes.begin(), boxes.end(), cb);
}

void SelfIntersection::clear() {
    m_intersecting_pairs.clear();
}

void SelfIntersection::handle_intersection_candidate(
        const TrianglesIterator& b1, const TrianglesIterator& b2) {
    const Triangle_3& t1 = *b1;
    const Triangle_3& t2 = *b2;
    size_t id1 = b1 - m_mesh.begin();
    size_t id2 = b2 - m_mesh.begin();

    size_t topo_overlap = topological_overlap(id1, id2);
    bool is_intersecting = false;
    switch (topo_overlap) {
        case 0:
            is_intersecting = CGAL::do_intersect(t1, t2);
            break;
        case 3:
            // duplicated face
            is_intersecting = true;
            break;
        default:
            {
                auto intersection = CGAL::intersection(t1, t2);
                if (intersection) {
                    if (topo_overlap == 1) {
                        const Point_3* p = boost::get<Point_3>(&*intersection);
                        is_intersecting = (p == NULL);
                    } else {
                        assert(topo_overlap == 2);
                        const Segment_3* s = boost::get<Segment_3>(&*intersection);
                        is_intersecting = (s == NULL);
                    }
                }
            }
    }

    if (is_intersecting) {
        m_intersecting_pairs.emplace_back(id1, id2);
    }
}

std::vector<Box> SelfIntersection::get_triangle_bboxes() {
    size_t num_faces = m_mesh.size();
    std::vector<Box> boxes;
    boxes.reserve(num_faces);
    for (TrianglesIterator itr=m_mesh.begin(); itr!=m_mesh.end(); itr++) {
        CGAL::Bbox_3 bbox = itr->bbox();
        boxes.push_back(Box(bbox, itr));
    }
    return boxes;
}

size_t SelfIntersection::topological_overlap(size_t id1, size_t id2) const {
    Vector3I f1 = m_faces.row(id1);
    Vector3I f2 = m_faces.row(id2);
    std::sort(f1.data(), f1.data()+3);
    std::sort(f2.data(), f2.data()+3);
    std::vector<int> unique_vertices(6, 0);
    auto itr = std::set_union(
            f1.data(), f1.data()+3, 
            f2.data(), f2.data()+3, unique_vertices.begin());
    return 6 - (itr - unique_vertices.begin());
}
