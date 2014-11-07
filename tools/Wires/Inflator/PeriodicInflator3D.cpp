#include "PeriodicInflator3D.h"

#include <limits>
#include <list>
#include <iostream>
#include <vector>

#include <CSG/CSGEngine.h>
#include <MeshUtils/TripletMap.h>
#include <Misc/HashGrid.h>
#include <Misc/Triplet.h>
extern "C" {
#include <Misc/tribox3.h>
}

#include <MeshFactory.h>
#include "PeriodicBoundaryRemesher.h"

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
}

using namespace PeriodicInflator3DHelper;

void PeriodicInflator3D::clip_to_center_cell() {
    clip_phantom_mesh();
    periodic_remesh();
    update_face_sources();
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

void PeriodicInflator3D::periodic_remesh() {
    Float ave_thickness = m_thickness.sum() / m_thickness.size();
    PeriodicBoundaryRemesher remesher(m_vertices, m_faces,
            m_center_cell_bbox_min,
            m_center_cell_bbox_max);
    remesher.remesh(ave_thickness);
    m_vertices = remesher.get_vertices();
    m_faces = remesher.get_faces();
}

