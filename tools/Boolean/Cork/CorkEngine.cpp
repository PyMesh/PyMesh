/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#ifdef WITH_CORK
#include "CorkEngine.h"

#include <algorithm>
#include <cassert>
#include <sstream>

#include <cork.h>

#include <Core/EigenTypedef.h>
#include <Core/Exception.h>

using namespace PyMesh;

namespace CorkEngineHelper {
    void ensure_3D_triangle_mesh(const MatrixFr& vertices, const MatrixIr& faces) {
        if (vertices.cols() != 3) {
            std::stringstream err_msg;
            err_msg << "Cork library only supports 3D mesh" << std::endl;
            err_msg << vertices.cols() << "D mesh detected.";
            throw NotImplementedError(err_msg.str());
        }

        if (faces.cols() != 3) {
            std::stringstream err_msg;
            err_msg << "Cork library only supports triangle mesh" << std::endl;
            err_msg << "Input mesh has " << faces.cols() << " vertices per face.";
            throw NotImplementedError(err_msg.str());
        }
    }

    CorkTriMesh create_cork_mesh(const MatrixFr& vertices, const MatrixIr& faces) {
        ensure_3D_triangle_mesh(vertices, faces);

        const size_t num_vertices = vertices.rows();
        const size_t num_faces = faces.rows();
        const size_t dim = vertices.cols();
        const size_t vertex_per_face = faces.cols();

        CorkTriMesh mesh;
        mesh.n_vertices = num_vertices;
        mesh.n_triangles = num_faces;


        mesh.vertices = new float[num_vertices * dim];
        mesh.triangles = new unsigned int[num_faces * vertex_per_face];

        std::copy(vertices.data(), vertices.data() + num_vertices*dim, mesh.vertices);
        std::copy(faces.data(), faces.data() + num_faces*vertex_per_face, mesh.triangles);

        return mesh;
    }

    void extract_data_from_cork_mesh(const CorkTriMesh& mesh, MatrixFr& vertices, MatrixIr& faces) {
        const size_t num_vertices = mesh.n_vertices;
        const size_t num_faces = mesh.n_triangles;
        const size_t dim = 3;
        const size_t vertex_per_face = 3;

        vertices.resize(num_vertices, dim);
        faces.resize(num_faces, vertex_per_face);

        std::copy(mesh.vertices, mesh.vertices + num_vertices*dim, vertices.data());
        std::copy(mesh.triangles, mesh.triangles + num_faces*vertex_per_face, faces.data());
    }

    void dispose_cork_mesh(CorkTriMesh& mesh) {
        freeCorkTriMesh(&mesh);
    }
}

using namespace CorkEngineHelper;

CorkEngine::~CorkEngine() {
    dispose_cork_mesh(m_mesh_1);
    dispose_cork_mesh(m_mesh_2);
}

void CorkEngine::compute_union() {
    CorkTriMesh result;

    computeUnion(m_mesh_1, m_mesh_2, &result);
    extract_data_from_cork_mesh(result, m_vertices, m_faces);

    dispose_cork_mesh(result);
    clean_up();
}

void CorkEngine::compute_intersection() {
    CorkTriMesh result;

    computeIntersection(m_mesh_1, m_mesh_2, &result);
    extract_data_from_cork_mesh(result, m_vertices, m_faces);

    dispose_cork_mesh(result);
    clean_up();
}

void CorkEngine::compute_difference() {
    CorkTriMesh result;

    computeDifference(m_mesh_1, m_mesh_2, &result);
    extract_data_from_cork_mesh(result, m_vertices, m_faces);

    dispose_cork_mesh(result);
    clean_up();
}

void CorkEngine::compute_symmetric_difference() {
    CorkTriMesh result;

    computeSymmetricDifference(m_mesh_1, m_mesh_2, &result);
    extract_data_from_cork_mesh(result, m_vertices, m_faces);

    dispose_cork_mesh(result);
    clean_up();
}

void CorkEngine::convert_mesh_to_native_format(MeshSelection s) {
    switch(s) {
        case MeshSelection::FIRST:
            dispose_cork_mesh(m_mesh_1);
            m_mesh_1 = create_cork_mesh(m_vertices_1, m_faces_1);
            break;
        case MeshSelection::SECOND:
            dispose_cork_mesh(m_mesh_2);
            m_mesh_2 = create_cork_mesh(m_vertices_2, m_faces_2);
            break;
        default:
            throw NotImplementedError("Invalid mesh selection!");
    }
}

#endif
