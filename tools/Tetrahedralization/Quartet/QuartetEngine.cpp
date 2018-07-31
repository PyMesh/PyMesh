#ifdef WITH_QUARTET
#include "QuartetEngine.h"

#include <vector>
#include <Core/Exception.h>

#include <make_signed_distance.h>
#include <make_tet_mesh.h>
#include <sdf.h>
#include <trimesh.h>
#include <vec.h>

using namespace PyMesh;

void QuartetEngine::run() {
    using VertexArray = std::vector<Vec3f>;
    using FaceArray = std::vector<Vec3i>;
    using TetArray = std::vector<Vec4i>;

    const size_t num_vertices = m_vertices.rows();
    const size_t num_faces = m_faces.rows();

    // Extract cell size based on edge length.
    preprocess();

    // Convert geometry to quartet TriMesh.
    VertexArray vertices(num_vertices);
    FaceArray faces(num_faces);
    for (size_t i=0; i<num_vertices; i++) {
        vertices[i] = Vec3f(
                m_vertices(i, 0), 
                m_vertices(i, 1), 
                m_vertices(i, 2));
    }
    for (size_t i=0; i<num_faces; i++) {
        faces[i] = Vec3i(
                m_faces(i, 0),
                m_faces(i, 1),
                m_faces(i, 2));
    }
    TriMesh trimesh(vertices, faces);

    // Convert TriMesh into signed distance field.
    // Quartet stuffs 46 tets into a 4x4x4 grid.  Assuming each tet is near
    // regular, the circumradius of each tet is
    // ((4^3/46)*6*sqrt(2))^(1/3)*sqrt(6)/4 = 1.3943712855 times dx;
    // Thus, dx should be 1/1.3943712855 ~=  0.71716910007
    float dx = m_cell_size * 0.71716910007 * 0.5;
    VectorF bbox_min = m_vertices.colwise().minCoeff();
    VectorF bbox_max = m_vertices.colwise().maxCoeff();
    Vec3f origin(bbox_min[0], bbox_min[1], bbox_min[2]);
    origin -= Vec3f(dx, dx, dx) * 2;
    int ni = (int)std::ceil((bbox_max[0] - bbox_min[0]) / dx) + 5;
    int nj = (int)std::ceil((bbox_max[1] - bbox_min[1]) / dx) + 5;
    int nk = (int)std::ceil((bbox_max[2] - bbox_min[2]) / dx) + 5;

    SDF sdf(origin, dx, ni, nj, nk);
    make_signed_distance(faces, vertices, sdf);

    // Feature extraction.
    FeatureSet features;
    features.autoDetectFeatures(trimesh, m_feature_angle);

    // Tet meshing!
    TetMesh mesh;
    make_tet_mesh(mesh, sdf, features,
            true,  // optimized
            false, // output intermediate meshes
            false  // unsafe feature matching, not sure what it means.
            );

    // Extract geometry.
    VertexArray& verts = mesh.verts();
    TetArray& tets = mesh.tets();

    const size_t out_num_vertices = verts.size();
    const size_t out_num_tets = tets.size();
    m_vertices.resize(out_num_vertices, 3);
    m_voxels.resize(out_num_tets, 4);
    m_faces.resize(0, 3);

    for (size_t i=0; i<out_num_vertices; i++) {
        m_vertices(i, 0) = verts[i][0];
        m_vertices(i, 1) = verts[i][1];
        m_vertices(i, 2) = verts[i][2];
    }
    for (size_t i=0; i<out_num_tets; i++) {
        m_voxels(i, 0) = tets[i][0];
        m_voxels(i, 1) = tets[i][1];
        m_voxels(i, 2) = tets[i][2];
        m_voxels(i, 3) = tets[i][3];
    }

    // Some sanity check.
    assert(out_num_tets == 0 || m_voxels.maxCoeff() < out_num_vertices);
}

#endif
