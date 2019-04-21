/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#ifdef WITH_CGAL
#ifdef WITH_CGAL_COREFINEMENT

#include "CGALCorefinementEngine.h"
#include "CGALUtils.h"

#include <vector>

#include <CGAL/Union_find.h>
#include <CGAL/Polygon_mesh_processing/corefinement.h>
#include <CGAL/Polygon_mesh_processing/triangulate_faces.h>
#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/Surface_mesh.h>

#include <Core/Exception.h>

using namespace PyMesh;

namespace CGALCorefinementEngineHelper {
    template<typename SurfaceMesh>
    SurfaceMesh mesh_to_surface_mesh(
            const MatrixFr& vertices, const MatrixIr& faces) {
        using Point = typename SurfaceMesh::Point;
        using VertexIndex = typename SurfaceMesh::Vertex_index;

        assert(vertices.cols() == 3);
        assert(faces.cols() == 3);

        SurfaceMesh out;
        const size_t num_vertices = vertices.rows();
        std::vector<VertexIndex> v_indices(num_vertices);
        for (size_t i=0; i<num_vertices; i++) {
            v_indices[i] = out.add_vertex({
                        vertices(i,0), vertices(i,1), vertices(i,2)});
        }

        const size_t num_faces = faces.rows();
        for (size_t i=0; i<num_faces; i++) {
            out.add_face(
                    v_indices[faces(i, 0)],
                    v_indices[faces(i, 1)],
                    v_indices[faces(i, 2)]);
        }

        return out;
    }

    template<typename SurfaceMesh>
    void surface_mesh_to_mesh(const SurfaceMesh& M,
            MatrixFr& vertices, MatrixIr& faces) {
        const size_t num_vertices = M.number_of_vertices();
        vertices.resize(num_vertices, 3);
        for(const auto vit : M.vertices()) {
            const auto& p = M.point(vit);
            vertices.row(vit) <<
                CGAL::to_double(p[0]),
                CGAL::to_double(p[1]),
                CGAL::to_double(p[2]);
        }

        const size_t num_faces = M.number_of_faces();
        faces.resize(num_faces, 3);
        for (const auto fit : M.faces()) {
            auto he = M.halfedge(fit);
            auto v0 = M.source(he);
            he = M.next(he);
            auto v1 = M.source(he);
            he = M.next(he);
            auto v2 = M.source(he);
            faces.row(fit) << v0, v1, v2;
        }
    }

}
using namespace CGALCorefinementEngineHelper;

void CGALCorefinementEngine::compute_union() {
    using namespace CGAL::Polygon_mesh_processing;
    SurfaceMesh out;

    corefine_and_compute_union(m_surface_mesh_1, m_surface_mesh_2, out);
    triangulate_faces(out);

    surface_mesh_to_mesh(out, m_vertices, m_faces);
}

void CGALCorefinementEngine::compute_intersection() {
    using namespace CGAL::Polygon_mesh_processing;
    SurfaceMesh out;

    corefine_and_compute_intersection(m_surface_mesh_1, m_surface_mesh_2, out);
    triangulate_faces(out);

    surface_mesh_to_mesh(out, m_vertices, m_faces);
}

void CGALCorefinementEngine::compute_difference() {
    using namespace CGAL::Polygon_mesh_processing;
    SurfaceMesh out;

    corefine_and_compute_difference(m_surface_mesh_1, m_surface_mesh_2, out);
    triangulate_faces(out);

    surface_mesh_to_mesh(out, m_vertices, m_faces);
}

void CGALCorefinementEngine::compute_symmetric_difference() {
    using namespace CGAL::Polygon_mesh_processing;
    SurfaceMesh diff12, diff21, out;

    corefine_and_compute_difference(m_surface_mesh_1, m_surface_mesh_2, diff12);
    corefine_and_compute_difference(m_surface_mesh_2, m_surface_mesh_1, diff21);
    corefine_and_compute_union(diff12, diff21, out);
    triangulate_faces(out);

    surface_mesh_to_mesh(out, m_vertices, m_faces);
}

void CGALCorefinementEngine::convert_mesh_to_native_format(MeshSelection s) {
    switch (s) {
        case MeshSelection::FIRST:
            {
                m_surface_mesh_1 = mesh_to_surface_mesh<SurfaceMesh>(
                        m_vertices_1, m_faces_1);
            }
            break;
        case MeshSelection::SECOND:
            {
                m_surface_mesh_2 = mesh_to_surface_mesh<SurfaceMesh>(
                        m_vertices_2, m_faces_2);
            }
            break;
        default:
            throw RuntimeError("Unsupported mesh selection!");
    };
}

#endif
#endif
