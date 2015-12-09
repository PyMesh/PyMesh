/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#include "CarveEngine.h"

#include <cassert>
#include <memory>
#include <vector>
#include <iostream>
#include <carve/carve.hpp>
#include <carve/poly.hpp>
#include <carve/csg.hpp>
#include <carve/csg_triangulator.hpp>

#include <Core/Exception.h>

namespace CarveEngineHelper {
    typedef carve::csg::CSG::meshset_t CarveMesh;
    typedef std::shared_ptr<CarveMesh> CarveMeshPtr;

    CarveMeshPtr create_mesh(const MatrixFr& vertices, const MatrixIr& faces) {
        typedef CarveMesh::vertex_t CarveVertex;
        typedef CarveMesh::face_t   CarveFace;

        const size_t num_vertices = vertices.rows();
        const size_t num_faces = faces.rows();
        assert(vertices.cols() == 3);
        assert(faces.cols() == 3);

        std::vector<CarveVertex> carve_vertices;
        std::vector<CarveFace *> carve_faces;

        for (size_t i=0; i<num_vertices; i++) {
            const auto& v = vertices.row(i);
            carve_vertices.push_back(
                    CarveVertex(carve::geom::VECTOR(v[0], v[1], v[2])));
        }

        for (size_t i=0; i<num_faces; i++) {
            const auto& f = faces.row(i);
            carve_faces.push_back(
                    new CarveFace(
                        &carve_vertices[f[0]],
                        &carve_vertices[f[1]],
                        &carve_vertices[f[2]]));
        }

        return CarveMeshPtr(new CarveMesh(carve_faces));
    }

    void extract_data(CarveMeshPtr mesh, MatrixFr& vertices, MatrixIr& faces) {
        typedef CarveMesh::vertex_t CarveVertex;
        const size_t num_vertices = mesh->vertex_storage.size();
        vertices.resize(num_vertices, 3);
        for (size_t i=0; i<num_vertices; i++) {
            const auto& v = mesh->vertex_storage[i];
            vertices(i,0) = v.v.x;
            vertices(i,1) = v.v.y;
            vertices(i,2) = v.v.z;
        }

        const size_t num_faces = mesh->faceEnd() - mesh->faceBegin();
        faces.resize(num_faces, 3);
        for (auto itr=mesh->faceBegin(); itr != mesh->faceEnd(); itr++) {
            std::vector<CarveVertex* > vts;
            (*itr)->getVertices(vts);
            assert(vts.size() == 3);

            // WARNING:
            // Here is my guess on how to extract vertex index.
            // Carve's documentation is not clear on how to do this.  
            const size_t fid = itr - mesh->faceBegin();
            faces(fid, 0) = vts[0] - &mesh->vertex_storage[0];
            faces(fid, 1) = vts[1] - &mesh->vertex_storage[0];
            faces(fid, 2) = vts[2] - &mesh->vertex_storage[0];
        }
    }
}
using namespace CarveEngineHelper;

void CarveEngine::compute_union() {
    CarveMeshPtr mesh_1 = create_mesh(m_vertices_1, m_faces_1);
    CarveMeshPtr mesh_2 = create_mesh(m_vertices_2, m_faces_2);
    carve::csg::CSG csg;
    csg.hooks.registerHook(
            new carve::csg::CarveTriangulatorWithImprovement,
            carve::csg::CSG::Hooks::PROCESS_OUTPUT_FACE_BIT);
    CarveMeshPtr r(csg.compute(
                mesh_1.get(), mesh_2.get(),
                carve::csg::CSG::UNION));
    extract_data(r, m_vertices, m_faces);
}

void CarveEngine::compute_intersection() {
    CarveMeshPtr mesh_1 = create_mesh(m_vertices_1, m_faces_1);
    CarveMeshPtr mesh_2 = create_mesh(m_vertices_2, m_faces_2);
    carve::csg::CSG csg;
    csg.hooks.registerHook(
            new carve::csg::CarveTriangulatorWithImprovement,
            carve::csg::CSG::Hooks::PROCESS_OUTPUT_FACE_BIT);
    CarveMeshPtr r(csg.compute(
                mesh_1.get(), mesh_2.get(),
                carve::csg::CSG::INTERSECTION));
    extract_data(r, m_vertices, m_faces);
}

void CarveEngine::compute_difference() {
    CarveMeshPtr mesh_1 = create_mesh(m_vertices_1, m_faces_1);
    CarveMeshPtr mesh_2 = create_mesh(m_vertices_2, m_faces_2);
    carve::csg::CSG csg;
    csg.hooks.registerHook(
            new carve::csg::CarveTriangulatorWithImprovement,
            carve::csg::CSG::Hooks::PROCESS_OUTPUT_FACE_BIT);
    CarveMeshPtr r(csg.compute(
                mesh_1.get(), mesh_2.get(),
                carve::csg::CSG::A_MINUS_B));
    extract_data(r, m_vertices, m_faces);
}

void CarveEngine::compute_symmetric_difference() {
    CarveMeshPtr mesh_1 = create_mesh(m_vertices_1, m_faces_1);
    CarveMeshPtr mesh_2 = create_mesh(m_vertices_2, m_faces_2);
    carve::csg::CSG csg;
    csg.hooks.registerHook(
            new carve::csg::CarveTriangulatorWithImprovement,
            carve::csg::CSG::Hooks::PROCESS_OUTPUT_FACE_BIT);
    CarveMeshPtr r(csg.compute(
                mesh_1.get(), mesh_2.get(),
                carve::csg::CSG::SYMMETRIC_DIFFERENCE));
    extract_data(r, m_vertices, m_faces);
}
