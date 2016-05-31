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

using namespace PyMesh;

namespace CarveEngineHelper {
    typedef carve::csg::CSG::meshset_t CarveMesh;
    typedef std::shared_ptr<CarveMesh> CarveMeshPtr;
    typedef CarveMesh::vertex_t CarveVertex;
    typedef CarveVertex::vector_t CarveVector;

    CarveMeshPtr create_mesh(const MatrixFr& vertices, const MatrixIr& faces) {
        const size_t num_vertices = vertices.rows();
        const size_t num_faces = faces.rows();

        if (vertices.cols() != 3) {
            throw NotImplementedError("Only 3D mesh is supported.");
        }
        if (faces.cols() != 3) {
            throw NotImplementedError("Only triangle mesh is supported.");
        }

        std::vector<CarveVector> points;

        for (size_t i=0; i<num_vertices; i++) {
            const auto& v = vertices.row(i);
            CarveVector p;
            p.v[0] = v[0];
            p.v[1] = v[1];
            p.v[2] = v[2];
            points.push_back(p);
        }

        std::vector<int> raw_faces;
        raw_faces.reserve(num_faces * 4);
        for (size_t i=0; i<num_faces; i++) {
            raw_faces.push_back(3);
            raw_faces.push_back(faces(i,0));
            raw_faces.push_back(faces(i,1));
            raw_faces.push_back(faces(i,2));
        }

        return CarveMeshPtr(new CarveMesh(points, num_faces, raw_faces));
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
