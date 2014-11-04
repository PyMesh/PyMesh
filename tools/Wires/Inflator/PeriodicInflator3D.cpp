#include "PeriodicInflator3D.h"

#include <limits>
#include <list>
#include <functional>
#include <iostream>
#include <vector>

#include <CSG/CSGEngine.h>
#include <MeshUtils/Boundary.h>
#include <MeshUtils/TripletMap.h>
#include <Misc/HashGrid.h>
#include <Misc/Triplet.h>
extern "C" {
#include <Misc/tribox3.h>
}
#include <triangle/TriangleWrapper.h>

#include <MeshFactory.h>

namespace PeriodicInflator3DHelper {
    enum Location { INSIDE, ON_BORDER, OUTSIDE };
    Location bbox_check(
            const Vector3F& bbox_min,
            const Vector3F& bbox_max,
            const Vector3F& p0,
            const Vector3F& p1,
            const Vector3F& p2) {
        Vector3F bbox_center = 0.5 * (bbox_min + bbox_max);
        Vector3F bbox_size = 0.5 * (bbox_max - bbox_min);

        bool inside = (
                (p0.array() < bbox_max.array()).all() &&
                (p0.array() > bbox_min.array()).all() &&
                (p1.array() < bbox_max.array()).all() &&
                (p1.array() > bbox_min.array()).all() &&
                (p2.array() < bbox_max.array()).all() &&
                (p2.array() > bbox_min.array()).all());
        if (inside) return INSIDE;

        const Float tri[3][3] = {
            {p0[0], p0[1], p0[2]},  {p1[0], p1[1], p1[2]},  {p2[0], p2[1], p2[2]}
        };
        bool overlap = triBoxOverlap(
                bbox_center.data(),
                bbox_size.data(),
                tri);
        if (!overlap) return OUTSIDE;
        else return ON_BORDER;
    }

    void create_box(const VectorF& bbox_min, const VectorF& bbox_max,
            MatrixFr& box_vertices, MatrixIr& box_faces) {
        box_vertices.resize(8, 3);
        box_faces.resize(12, 3);
        box_vertices << bbox_min[0], bbox_min[1], bbox_min[2],
                        bbox_max[0], bbox_min[1], bbox_min[2],
                        bbox_max[0], bbox_max[1], bbox_min[2],
                        bbox_min[0], bbox_max[1], bbox_min[2],
                        bbox_min[0], bbox_min[1], bbox_max[2],
                        bbox_max[0], bbox_min[1], bbox_max[2],
                        bbox_max[0], bbox_max[1], bbox_max[2],
                        bbox_min[0], bbox_max[1], bbox_max[2];
        box_faces << 1, 2, 5,
                     5, 2, 6,
                     3, 4, 7,
                     3, 0, 4,
                     2, 3, 6,
                     3, 7, 6,
                     0, 1, 5,
                     0, 5, 4,
                     4, 5, 6,
                     4, 6, 7,
                     0, 3, 2,
                     0, 2, 1;
    }

    Mesh::Ptr form_mesh(const MatrixFr& vertices, const MatrixIr& faces) {
        const size_t v_size = vertices.rows() * vertices.cols();
        const size_t f_size = faces.rows() * faces.cols();

        VectorF flattened_vertices(v_size);
        std::copy(vertices.data(), vertices.data() + v_size,
                flattened_vertices.data());

        VectorI flattened_faces(f_size);
        std::copy(faces.data(), faces.data() + f_size,
                flattened_faces.data());

        VectorI voxels = VectorI::Zero(0);

        MeshFactory factory;
        factory.load_data(flattened_vertices, flattened_faces, voxels, 
                vertices.cols(), faces.cols(), 0);
        return factory.create_shared();
    }

    HashGrid::Ptr form_vertex_lookup_grid(const MatrixFr& vertices) {
        const size_t num_vertices = vertices.rows();
        HashGrid::Ptr grid = HashGrid::create(1e-6, 3);

        for (size_t i=0; i<num_vertices; i++) {
            const VectorF& v = vertices.row(i);
            grid->insert(i, v);
        }

        return grid;
    }

    TripletMap<size_t> form_face_lookup_map(const MatrixIr& faces,
            const std::vector<Location>& locations) {
        const size_t num_faces = faces.rows();
        assert(num_faces == locations.size());
        TripletMap<size_t> face_map;
        for (size_t i=0; i<num_faces; i++) {
            if (locations[i] == OUTSIDE) continue;
            const VectorI& f = faces.row(i);
            Triplet key(f[0], f[1], f[2]);
            face_map.insert(key, i);
        }
        return face_map;
    }

    bool look_up_triangle_index(
            HashGrid::Ptr grid,
            const TripletMap<size_t> face_map,
            const VectorF& v0,
            const VectorF& v1,
            const VectorF& v2,
            size_t& v_index,
            size_t& result) {
        VectorI index_0 = grid->get_items_near_point(v0);
        VectorI index_1 = grid->get_items_near_point(v1);
        VectorI index_2 = grid->get_items_near_point(v2);

        if (index_0.size() == 1) { v_index = index_0[0]; }
        if (index_1.size() == 1) { v_index = index_1[0]; }
        if (index_2.size() == 1) { v_index = index_2[0]; }

        if (index_0.size() == 0 || index_1.size() == 0 || index_2.size() == 0) {
            return false;
        }

        if (index_0.size() != 1 || index_1.size() != 1 || index_2.size() != 1) {
            throw RuntimeError("Duplicated vertex detected.");
        }

        Triplet key(index_0[0], index_1[0], index_2[0]);
        auto itr = face_map.find(key);
        if (itr == face_map.end()) return false;
        if (itr->second.size() != 1) {
            throw RuntimeError("Duplicated face detected.");
        }

        result = itr->second[0];
        return true;
    }

    bool point_is_in_triangle(const Vector3F& p,
            const Vector3F& c0, const Vector3F& c1, const Vector3F& c2) {
        // Note tolerance are in unit mm.
        const Float EPS = 1e-3;
        const Vector3F n = (c1 - c0).cross(c2 - c0).normalized();
        Float dist_to_plane = (p - c0).dot(n);
        if (dist_to_plane > EPS) return false;

        const Vector3F v0 = c0 - p;
        const Vector3F v1 = c1 - p;
        const Vector3F v2 = c2 - p;

        Float area_0 = v1.cross(v2).dot(n);
        Float area_1 = v2.cross(v0).dot(n);
        Float area_2 = v0.cross(v1).dot(n);

        return ((area_0 > -EPS) && (area_1 > -EPS) && (area_2 > -EPS));
    }

    std::vector<Location> compute_face_locations(
            const MatrixFr& vertices, const MatrixIr& faces,
            const WireNetwork& network) {
        const MatrixFr& wire_vertices = network.get_vertices();
        const VectorF bbox_min = wire_vertices.colwise().minCoeff();
        const VectorF bbox_max = wire_vertices.colwise().maxCoeff();
        const size_t num_faces = faces.rows();
        std::vector<Location> results(num_faces);
        for (size_t i=0; i<num_faces; i++) {
            const VectorI& f = faces.row(i);
            const VectorF& v0 = vertices.row(f[0]);
            const VectorF& v1 = vertices.row(f[1]);
            const VectorF& v2 = vertices.row(f[2]);
            results[i] = bbox_check(bbox_min, bbox_max, v0, v1, v2);
        }
        return results;
    }

    void refine_triangulation(
            MatrixFr& vertices, MatrixIr& faces, Float max_area) {
        // Apparently triangle sucks at refining poor triangulations.
        TriangleWrapper tri_wrapper(vertices, faces);
        tri_wrapper.run(max_area, false);
        vertices = tri_wrapper.get_vertices();
        faces = tri_wrapper.get_faces();
    }

    typedef std::list<std::function<bool(const Vector3F&)> > SegmentChecks;
    SegmentChecks compute_edge_check_functions(
            const VectorF& bbox_min, const VectorF& bbox_max) {
        const Float EPS = 1e-3;
        SegmentChecks checks;

        //   3 ________ 2
        //    /:      /|
        // 7 / :   6 / |
        //  +-------+  |
        //  |  :    |  |
        //  | 0:....|..| 1
        //  |.'     | /
        //  +-------+/
        // 4          5
        Vector3F base_points[] = {
            Vector3F(bbox_min[0], bbox_min[1], bbox_min[2]),
            Vector3F(bbox_max[0], bbox_min[1], bbox_min[2]),
            Vector3F(bbox_max[0], bbox_max[1], bbox_min[2]),
            Vector3F(bbox_min[0], bbox_max[1], bbox_min[2]),
            Vector3F(bbox_min[0], bbox_min[1], bbox_max[2]),
            Vector3F(bbox_max[0], bbox_min[1], bbox_max[2]),
            Vector3F(bbox_max[0], bbox_max[1], bbox_max[2]),
            Vector3F(bbox_min[0], bbox_max[1], bbox_max[2])
        };

        MatrixI edges(12, 2);
        edges << 0, 1,
                 3, 2,
                 7, 6,
                 4, 5,
                 0, 3,
                 1, 2,
                 5, 6,
                 4, 7,
                 0, 4,
                 1, 5,
                 2, 6,
                 3, 7;


        for (size_t i=0; i<12; i++) {
            const VectorI& e = edges.row(i);
            checks.push_back([=](const Vector3F& p) {
                    Vector3F v0 = p - base_points[e[0]];
                    Vector3F v1 = base_points[e[1]] - base_points[e[0]];
                    Float l0 = v0.norm();
                    Float l1 = v1.norm();
                    if (l0 > EPS) v0 /= l0;
                    if (l1 > EPS) v1 /= l1;
                    return v0.cross(v1).norm() < EPS; });
        }

        return checks;
    }

    void refine_boundaries(MatrixFr& vertices, MatrixIr& segments,
            const VectorF& bbox_min, const VectorF& bbox_max, Float ave_length) {
        const size_t num_vertices = vertices.rows();
        const size_t num_segments = segments.rows();

        std::list<Float> refined_vertices;
        std::list<size_t> refined_segments;

        for (size_t i=0; i<num_vertices; i++) {
            const VectorF& v = vertices.row(i);
            refined_vertices.push_back(v[0]);
            refined_vertices.push_back(v[1]);
            refined_vertices.push_back(v[2]);
        }

        SegmentChecks on_segments = compute_edge_check_functions(
                bbox_min, bbox_max);
        size_t vertex_count = num_vertices;
        for (size_t i=0; i<num_segments; i++) {
            const VectorI& edge = segments.row(i);
            const VectorF& v0 = vertices.row(edge[0]);
            const VectorF& v1 = vertices.row(edge[1]);

            bool need_refine = false;
            for (auto on_segment : on_segments) {
                if (on_segment(v0) && on_segment(v1)) {
                    need_refine = true;
                    break;
                }
            }

            Float edge_length = (v0 - v1).norm();
            if (!need_refine || edge_length < 2 * ave_length) {
                refined_segments.push_back(edge[0]);
                refined_segments.push_back(edge[1]);
            } else {
                size_t num_pieces = size_t(std::round(edge_length / ave_length));
                Float segment_length = edge_length / num_pieces;
                std::vector<size_t> nodes;
                nodes.push_back(edge[0]);
                for (size_t j=1; j<num_pieces; j++) {
                    Float ratio = Float(j)/Float(num_pieces);
                    VectorF v = v0 * (1.0 - ratio) + v1 * ratio;
                    refined_vertices.push_back(v[0]);
                    refined_vertices.push_back(v[1]);
                    refined_vertices.push_back(v[2]);
                    nodes.push_back(vertex_count);
                    vertex_count++;
                }
                nodes.push_back(edge[1]);
                for (size_t j=0; j<num_pieces; j++) {
                    refined_segments.push_back(nodes[j]);
                    refined_segments.push_back(nodes[j+1]);
                }
            }
        }

        size_t num_refined_vertices = refined_vertices.size() / 3;
        size_t num_refined_segments = refined_segments.size() / 2;
        vertices.resize(num_refined_vertices, 3);
        segments.resize(num_refined_segments, 2);
        std::copy(refined_vertices.begin(), refined_vertices.end(),
                vertices.data());
        std::copy(refined_segments.begin(), refined_segments.end(),
                segments.data());
    }

    void triangulate_boundary_loops(
            MatrixFr& vertices, MatrixIr& faces, Float ave_edge_len,
            const VectorF& bbox_min, const VectorF& bbox_max) {
        Boundary::Ptr bd_extractor = Boundary::extract_surface_boundary_raw(
                vertices, faces);
        MatrixIr boundaries = bd_extractor->get_boundaries();
        refine_boundaries(vertices, boundaries, bbox_min, bbox_max,
                ave_edge_len);

        TriangleWrapper tri_wrapper(vertices, boundaries);
        tri_wrapper.run(ave_edge_len * ave_edge_len, false, true);
        vertices = tri_wrapper.get_vertices();
        faces = tri_wrapper.get_faces();
    }
}

using namespace PeriodicInflator3DHelper;

void PeriodicInflator3D::clip_to_center_cell() {
    clip_phantom_mesh();
}

void PeriodicInflator3D::clip_phantom_mesh() {
    VectorF& bbox_min = m_center_cell_bbox_min;
    VectorF& bbox_max = m_center_cell_bbox_max;
    get_center_cell_bbox(bbox_min, bbox_max);

    MatrixFr box_vertices;
    MatrixIr box_faces;
    create_box(bbox_min, bbox_max, box_vertices, box_faces);

    CSGEngine::Ptr csg_engine = CSGEngine::create("cork");
    csg_engine->set_mesh_1(m_phantom_vertices, m_phantom_faces);
    csg_engine->set_mesh_2(box_vertices, box_faces);
    csg_engine->compute_intersection();

    m_vertices = csg_engine->get_vertices();
    m_faces = csg_engine->get_faces();
    m_face_sources = VectorI(m_faces.rows());

    enforce_periodicity();
    update_face_sources();
}

void PeriodicInflator3D::update_face_sources() {
    const size_t num_phantom_vertices = m_phantom_vertices.rows();
    const size_t num_phantom_faces = m_phantom_faces.rows();

    Mesh::Ptr phantom_mesh = form_mesh(m_phantom_vertices, m_phantom_faces);
    phantom_mesh->enable_vertex_connectivity();

    std::vector<Location> phantom_face_locations = compute_face_locations(
            m_phantom_vertices, m_phantom_faces, m_wire_network);
    HashGrid::Ptr grid = form_vertex_lookup_grid(m_phantom_vertices);
    TripletMap<size_t> face_map = form_face_lookup_map(m_phantom_faces,
            phantom_face_locations);

    const size_t num_faces = m_faces.rows();
    m_face_sources = VectorI::Zero(num_faces);
    for (size_t i=0; i<num_faces; i++) {
        const VectorI& f = m_faces.row(i);
        const VectorF v0 = m_vertices.row(f[0]);
        const VectorF v1 = m_vertices.row(f[1]);
        const VectorF v2 = m_vertices.row(f[2]);
        size_t face_index = std::numeric_limits<size_t>::max();
        size_t v_index = std::numeric_limits<size_t>::max();
        bool is_not_clipped = look_up_triangle_index(
                grid, face_map, v0, v1, v2, v_index, face_index);
        if (is_not_clipped) {
            assert(face_index < num_phantom_faces);
            m_face_sources[i] = m_phantom_face_sources[face_index];
        } else if (v_index < num_phantom_vertices){
            Vector3F centroid = (v0+v1+v2) / 3.0;
            VectorI adj_faces = phantom_mesh->get_vertex_adjacent_faces(v_index);
            const size_t num_adj_faces = adj_faces.size();
            assert(num_adj_faces > 0);
            for (size_t j=0; j<num_adj_faces; j++) {
                const VectorI& tri = m_phantom_faces.row(adj_faces[j]);
                const Vector3F& corner_0 = m_phantom_vertices.row(tri[0]);
                const Vector3F& corner_1 = m_phantom_vertices.row(tri[1]);
                const Vector3F& corner_2 = m_phantom_vertices.row(tri[2]);
                if (point_is_in_triangle(centroid, corner_0, corner_1, corner_2)) {
                    m_face_sources[i] = m_phantom_face_sources[adj_faces[j]];
                    break;
                }
            }
        }
    }
}

void PeriodicInflator3D::enforce_periodicity() {
    const size_t num_faces = m_faces.rows();
    BDLabels boundary_face_labels(num_faces, 0);

    label_boundary_faces(0, boundary_face_labels);
    label_boundary_faces(1, boundary_face_labels);
    label_boundary_faces(2, boundary_face_labels);

    VectorF face_labels(num_faces);
    std::copy(boundary_face_labels.begin(), boundary_face_labels.end(),
            face_labels.data());
    save_mesh("face_labels.msh", m_vertices, m_faces, face_labels);

    VertexList vertices;
    FaceList faces;
    extract_interior_faces(boundary_face_labels, vertices, faces);
    retriangulate_boundaries(boundary_face_labels, vertices, faces);

    assert(vertices.size() % 3 == 0);
    assert(faces.size() % 3 == 0);

    const size_t num_remeshed_vertices = vertices.size() / 3;
    const size_t num_remeshed_faces = faces.size() / 3;
    m_vertices.resize(num_remeshed_vertices, 3);
    m_faces.resize(num_remeshed_faces, 3);

    std::copy(vertices.begin(), vertices.end(), m_vertices.data());
    std::copy(faces.begin(), faces.end(), m_faces.data());
}

void PeriodicInflator3D::label_boundary_faces(size_t axis,
        PeriodicInflator3D::BDLabels& boundary_face_labels) {
    const Float EPS = 1e-3;
    const VectorF& bbox_min = m_center_cell_bbox_min;
    const VectorF& bbox_max = m_center_cell_bbox_max;

    const size_t num_vertices = m_vertices.rows();
    const size_t num_faces = m_faces.rows();

    std::vector<short> on_boundary(num_vertices, 0);
    for (size_t i=0; i<num_vertices; i++) {
        const VectorF& v = m_vertices.row(i);
        if (fabs(v[axis] - bbox_min[axis]) < EPS) {
            on_boundary[i] = -1;
        }
        if (fabs(v[axis] - bbox_max[axis]) < EPS) {
            on_boundary[i] = 1;
        }
    }

    for (size_t i=0; i<num_faces; i++) {
        const VectorI& f = m_faces.row(i);
        Vector3I face_on_boundary(
                on_boundary[f[0]],
                on_boundary[f[1]],
                on_boundary[f[2]]);
        if ((face_on_boundary.array() > 0).all()) {
            boundary_face_labels[i] = int(axis+1);
        } else if ((face_on_boundary.array() < 0).all()) {
            boundary_face_labels[i] = -int(axis+1);
        }
    }
}

void PeriodicInflator3D::extract_interior_faces(
        const PeriodicInflator3D::BDLabels& bd_labels,
        PeriodicInflator3D::VertexList& v_list,
        PeriodicInflator3D::FaceList& f_list) {
    const size_t num_vertices = m_vertices.rows();
    for (size_t i=0; i<num_vertices; i++) {
        const VectorF& v = m_vertices.row(i);
        v_list.push_back(v[0]);
        v_list.push_back(v[1]);
        v_list.push_back(v[2]);
    }

    const size_t num_faces = bd_labels.size();
    for (size_t i=0; i<num_faces; i++) {
        if (bd_labels[i] != 0) continue;
        const VectorI& f = m_faces.row(i);
        f_list.push_back(f[0]);
        f_list.push_back(f[1]);
        f_list.push_back(f[2]);
    }
}

void PeriodicInflator3D::retriangulate_boundaries(
        const PeriodicInflator3D::BDLabels& bd_labels,
        PeriodicInflator3D::VertexList& v_list,
        PeriodicInflator3D::FaceList& f_list) {
    Float ave_thickness = m_thickness.sum() / m_thickness.size();
    VectorF bbox_size = m_center_cell_bbox_max - m_center_cell_bbox_min;

    const int labels[] = {-3, -2, -1};
    const size_t axis_index[] = {2, 1, 0};
    const Vector3F offsets[] = {
        Vector3F(0.0, 0.0, bbox_size[2]),
        Vector3F(0.0, bbox_size[1], 0.0),
        Vector3F(bbox_size[0], 0.0, 0.0)
    };

    size_t base_index = v_list.size() / 3;
    for (size_t i=0; i<3; i++) {
        MatrixFr vertices;
        MatrixIr faces;
        extract_boundary_faces(bd_labels, labels[i], vertices, faces);
        vertices.block(0, axis_index[i], vertices.rows(), 1).array() =
            VectorF::Ones(vertices.rows()) * m_center_cell_bbox_min[axis_index[i]];
        save_mesh("bd_face.msh", vertices, faces, VectorF::Ones(faces.rows()));
        triangulate_boundary_loops(vertices, faces, ave_thickness,
                m_center_cell_bbox_min, m_center_cell_bbox_max);
        save_mesh("bd_tri_face.msh", vertices, faces, VectorF::Ones(faces.rows()));

        const size_t num_vertices = vertices.rows();
        const size_t num_faces = faces.rows();

        for (size_t j=0; j<num_vertices; j++) {
            v_list.push_back(vertices(j, 0));
            v_list.push_back(vertices(j, 1));
            v_list.push_back(vertices(j, 2));
        }
        for (size_t j=0; j<num_faces; j++) {
            f_list.push_back(faces(j, 0) + base_index);
            f_list.push_back(faces(j, 1) + base_index);
            f_list.push_back(faces(j, 2) + base_index);
        }
        base_index += num_vertices;

        for (size_t j=0; j<num_vertices; j++) {
            v_list.push_back(vertices(j, 0) + offsets[i][0]);
            v_list.push_back(vertices(j, 1) + offsets[i][1]);
            v_list.push_back(vertices(j, 2) + offsets[i][2]);
        }
        for (size_t j=0; j<num_faces; j++) {
            f_list.push_back(faces(j, 1) + base_index);
            f_list.push_back(faces(j, 0) + base_index);
            f_list.push_back(faces(j, 2) + base_index);
        }
        base_index += num_vertices;
    }
}

void PeriodicInflator3D::extract_boundary_faces(
        const PeriodicInflator3D::BDLabels& bd_labels, int label,
        MatrixFr& vertices, MatrixIr& faces) {
    const size_t num_vertices = m_vertices.rows();
    const size_t num_faces = m_faces.rows();
    assert(bd_labels.size() == num_faces);

    std::vector<int> index_map(num_vertices, -1);
    std::list<size_t> bd_faces;

    std::list<Float> raw_vertices;
    std::list<size_t> raw_faces;

    size_t count = 0;
    for (size_t i=0; i<num_faces; i++) {
        if (bd_labels[i] == label) {
            const VectorI& f = m_faces.row(i);
            bd_faces.push_back(i);
            for (size_t j=0; j<3; j++) {
                if (index_map[f[j]] == -1) {
                    index_map[f[j]] = count;
                    const VectorF& v = m_vertices.row(f[j]);
                    raw_vertices.push_back(v[0]);
                    raw_vertices.push_back(v[1]);
                    raw_vertices.push_back(v[2]);
                    count++;
                }
                raw_faces.push_back(index_map[f[j]]);
            }
        }
    }

    vertices.resize(count, 3);
    std::copy(raw_vertices.begin(), raw_vertices.end(), vertices.data());

    faces.resize(bd_faces.size(), 3);
    std::copy(raw_faces.begin(), raw_faces.end(), faces.data());
}


