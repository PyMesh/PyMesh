#include "BSPEngine.h"

#include <cassert>
#include <vector>
#include <iostream>
#include <Core/Exception.h>

#include <bsp.hpp>
#include <plyread.hpp>
#include <metricPolygon.hpp>
#include <convexPolygon.hpp>

using namespace PyMesh;

namespace BSPEngineHelper {
    typedef std::vector<BSPlib::ConvexPolygon> BRep;
    typedef boost::shared_ptr<BSPlib::Bsp> BSPPtr;

    BSPPtr raw_to_bsp(const MatrixFr& vertices, const MatrixIr& faces) {
        const size_t num_vertices = vertices.rows();
        const size_t num_faces = faces.rows();
        assert(vertices.cols() == 3);
        assert(faces.cols() == 3);

        PLYmesh mesh;
        mesh.vertices.resize(num_vertices * 3);
        std::copy(vertices.data(), vertices.data() + num_vertices*3,
                mesh.vertices.begin());

        mesh.faces.resize(num_faces);
        for (size_t i=0; i<num_faces; i++) {
            mesh.faces[i] = {faces(i,0), faces(i,1), faces(i,2)};
        }
        BRep brep;
        Ply2Convex(mesh, brep);
        {
            auto r = BSPlib::Bsp::BuildTree(brep);
            auto convex_polys = r->GetBoundary();
            std::vector<BSPlib::MetricPolygon> metric_polys(convex_polys.size());
            std::transform(convex_polys.begin(), convex_polys.end(),
                    metric_polys.begin(), [](const BSPlib::ConvexPolygon& p) {
                    return BSPlib::Approximate(p); });
            PLYmesh tmp;
            Metric2Ply(metric_polys, tmp);
            Write("tmp.ply", tmp);
        }
        return BSPlib::Bsp::BuildTree(brep);
    }

    void bsp_to_raw(const BSPPtr bsp, MatrixFr& vertices, MatrixIr& faces) {
        const auto convex_polys = bsp->GetBoundary();
        std::vector<BSPlib::MetricPolygon> metric_polys(convex_polys.size());
        std::transform(convex_polys.begin(), convex_polys.end(),
                metric_polys.begin(), [](const BSPlib::ConvexPolygon& p) {
                return BSPlib::Approximate(p); });
        PLYmesh mesh;
        Metric2Ply(metric_polys, mesh);
        Write("out.ply", mesh);

        const size_t num_vertices = mesh.vertices.size() / 3;
        assert(mesh.vertices.size() % 3 == 0);
        vertices.resize(num_vertices, 3);
        std::copy(mesh.vertices.begin(), mesh.vertices.end(),
                vertices.data());

        const size_t num_faces = mesh.faces.size();
        std::vector<int> triangles;
        for (size_t i=0; i<num_faces; i++) {
            const auto f = mesh.faces[i];
            if (f.size() == 3) {
                triangles.insert(triangles.end(), f.begin(), f.end());
            } else if (f.size() == 4) {
                triangles.push_back(f[0]);
                triangles.push_back(f[1]);
                triangles.push_back(f[2]);

                triangles.push_back(f[0]);
                triangles.push_back(f[2]);
                triangles.push_back(f[3]);
            } else {
                const size_t vertex_per_face = f.size();
                for (size_t vid=2; vid<vertex_per_face; vid++) {
                    triangles.push_back(f[0]);
                    triangles.push_back(f[vid-1]);
                    triangles.push_back(f[vid]);
                }
            }
        }
        faces.resize(triangles.size()/3, 3);
        std::copy(triangles.begin(), triangles.end(), faces.data());
    }
}

using namespace BSPEngineHelper;

void BSPEngine::compute_union() {
    BSPPtr mesh1 = raw_to_bsp(m_vertices_1, m_faces_1);
    BSPPtr mesh2 = raw_to_bsp(m_vertices_2, m_faces_2);
    BSPPtr result = BSPlib::Bsp::Union(mesh1, mesh2);
    bsp_to_raw(result, m_vertices, m_faces);
}

void BSPEngine::compute_intersection() {
    BSPPtr mesh1 = raw_to_bsp(m_vertices_1, m_faces_1);
    BSPPtr mesh2 = raw_to_bsp(m_vertices_2, m_faces_2);
    BSPPtr result = BSPlib::Bsp::Intersection(mesh1, mesh2);
    bsp_to_raw(result, m_vertices, m_faces);
}

void BSPEngine::compute_difference() {
    BSPPtr mesh1 = raw_to_bsp(m_vertices_1, m_faces_1);
    BSPPtr mesh2 = raw_to_bsp(m_vertices_2, m_faces_2);
    BSPPtr result = BSPlib::Bsp::Difference(mesh1, mesh2);
    bsp_to_raw(result, m_vertices, m_faces);
}

void BSPEngine::compute_symmetric_difference() {
    BSPPtr mesh1 = raw_to_bsp(m_vertices_1, m_faces_1);
    BSPPtr mesh2 = raw_to_bsp(m_vertices_2, m_faces_2);
    BSPPtr left = BSPlib::Bsp::Difference(mesh1, mesh2);
    BSPPtr right = BSPlib::Bsp::Difference(mesh1, mesh2);
    BSPPtr result = BSPlib::Bsp::Union(left, right);
    bsp_to_raw(result, m_vertices, m_faces);
}
