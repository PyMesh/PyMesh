/* This file is part of PyMesh. Copyright (c) 2018 by Qingnan Zhou */
#if defined(WITH_MMG) && defined(WITH_TETGEN) && defined(WITH_IGL)

#include "MMGEngine.h"
#include <Core/Exception.h>
#include <TetGen/TetgenWrapper.h>
extern "C" {
#include <mmg/libmmg.h>
}
#include <igl/signed_distance.h>

using namespace PyMesh;

void MMGEngine::run() {
    constexpr Float EPS = 0.1;
    const Vector3F bbox_min = m_vertices.colwise().minCoeff().array() - EPS;
    const Vector3F bbox_max = m_vertices.colwise().maxCoeff().array() + EPS;

    MatrixFr cube_vertices(8, 3);
    cube_vertices <<
        bbox_min[0], bbox_min[1], bbox_min[2],
        bbox_max[0], bbox_min[1], bbox_min[2],
        bbox_max[0], bbox_max[1], bbox_min[2],
        bbox_min[0], bbox_max[1], bbox_min[2],
        bbox_min[0], bbox_min[1], bbox_max[2],
        bbox_max[0], bbox_min[1], bbox_max[2],
        bbox_max[0], bbox_max[1], bbox_max[2],
        bbox_min[0], bbox_max[1], bbox_max[2];
    MatrixIr cube_faces(12, 3);
    cube_faces << 0, 2, 1,
                  0, 3, 2,
                  4, 5, 6,
                  4, 6, 7,
                  1, 2, 6,
                  1, 6, 5,
                  0, 4, 7,
                  0, 7, 3,
                  0, 1, 5,
                  0, 5, 4,
                  2, 3, 6,
                  6, 3, 7;

    // Use the volume of regular tetrahedron of radius m_cell_size as max
    // volume.
    const Float max_tet_volume = 0.1 * 8 * pow(m_cell_size, 3) / (9 * sqrt(3));

    // Generate ambient mesh.
    TetgenWrapper tetgen;
    tetgen.set_points(cube_vertices);
    tetgen.set_triangles(cube_faces);
    tetgen.set_max_tet_volume(max_tet_volume);
    tetgen.set_verbosity(0);
    tetgen.run();

    auto ambient_vertices = tetgen.get_vertices();
    auto ambient_tets = tetgen.get_voxels();
    ambient_tets.array() += 1;// Sigh... one based index.
    const size_t num_ambient_vertices = ambient_vertices.rows();
    const size_t num_ambient_tets = ambient_tets.rows();
    const size_t num_faces = m_faces.rows();

    VectorI vertex_refs(num_ambient_vertices);
    VectorI tet_refs(num_ambient_tets);
    vertex_refs.setZero();
    tet_refs.setZero();

    // Compute signed distance function
    VectorF s;
    VectorI face_ids;
    MatrixF closest_pts, normals;
    igl::signed_distance(ambient_vertices, m_vertices, m_faces,
            igl::SIGNED_DISTANCE_TYPE_WINDING_NUMBER,
            s, face_ids, closest_pts, normals);
    assert(s.size() == num_ambient_vertices);

    // Initialize MMG3D
    int r = 0;
    MMG5_pMesh mmgMesh = NULL;
    MMG5_pSol mmgSol = NULL;
    r = MMG3D_Init_mesh(MMG5_ARG_start,
            MMG5_ARG_ppMesh,&mmgMesh, // Ambient mesh.
            MMG5_ARG_ppMet,&mmgSol,    // Level set.
            MMG5_ARG_end);
    if (!r) { throw RuntimeError("MMG3D_Init_mesh function failed."); }
    r = MMG3D_Set_meshSize(mmgMesh,
            num_ambient_vertices, // num points
            num_ambient_tets,     // num tets
            0, // num prisms
            0, // num triangles
            0, // num quads
            0); // num edges
    if (!r) { throw RuntimeError("MMG3D_Set_meshSize function failed."); }

    r = MMG3D_Set_vertices(mmgMesh, const_cast<Float*>(ambient_vertices.data()),
            vertex_refs.data());
    if (!r) { throw RuntimeError("MMG3D_Set_vertices function failed."); }
    r = MMG3D_Set_tetrahedra(mmgMesh, const_cast<int*>(ambient_tets.data()),
            tet_refs.data());
    if (!r) { throw RuntimeError("MMG3D_Set_tetrahedra function failed."); }
    //r = MMG3D_Set_triangles(mmgMesh, m_faces.data(), NULL);
    //if (!r) { throw RuntimeError("MMG3D_Set_triangle function failed."); }

    r = MMG3D_Set_iparameter(mmgMesh,mmgSol,MMG3D_IPARAM_iso, 1);
    if (!r) { throw RuntimeError("MMG3D_Set_iparameter function failed."); }
    r = MMG3D_Set_dparameter(mmgMesh,mmgSol,MMG3D_DPARAM_ls, 0.0);
    if (!r) { throw RuntimeError("MMG3D_Set_dparameter function failed."); }

    r = MMG3D_Set_solSize(mmgMesh, mmgSol, MMG5_Vertex,
            num_ambient_vertices, MMG5_Scalar);
    if (!r) { throw RuntimeError("MMG3D_Set_solSize function failed."); }
    r = MMG3D_Set_scalarSols(mmgSol, s.data());
    if (!r) { throw RuntimeError("MMG3D_Set_scalarSols function failed."); }
    r = MMG3D_Chk_meshData(mmgMesh,mmgSol);
    if (!r) { throw RuntimeError("MMG3D_Chk_meshData function failed."); }

    r = MMG3D_mmg3dls(mmgMesh,mmgSol);
    if (r == MMG5_STRONGFAILURE) {
        throw RuntimeError("MMG strong failure");
    } else if (r == MMG5_LOWFAILURE) {
        throw RuntimeError("MMG low failure");
    }

    int np,ne,nprism,nt,nquad,na,num_sol,sol_entity_type,sol_type;
    r = MMG3D_Get_meshSize(mmgMesh, &np, &ne, &nprism, &nt, &nquad, &na);
    if (!r) { throw RuntimeError("MMG3D_Get_meshSize function failed."); }

    MatrixFr tet_vertices(np, 3);
    MatrixIr tet_voxels(ne, 4);
    VectorI v_refs(np);
    VectorI t_refs(ne);
    VectorI are_corners(np);
    VectorI v_required(np);
    VectorI t_required(ne);
    r = MMG3D_Get_vertices(mmgMesh,
            tet_vertices.data(),
            v_refs.data(),
            are_corners.data(),
            v_required.data());
    if (!r) { throw RuntimeError("MMG3D_Get_vertices function failed."); }
    r = MMG3D_Get_tetrahedra(mmgMesh,
            tet_voxels.data(),
            t_refs.data(),
            t_required.data());
    if (!r) { throw RuntimeError("MMG3D_Get_tetrahedra function failed."); }

    MMG3D_Free_all(MMG5_ARG_start,
            MMG5_ARG_ppMesh,&mmgMesh,MMG5_ARG_ppMet,&mmgSol,
            MMG5_ARG_end);

    tet_voxels.array() -= 1; // 1-based index to 0-based.

    m_voxels.resize(ne, 4);
    size_t count= 0;
    for (size_t i=0; i<ne; i++) {
        if (t_refs[i] == 3) {
            m_voxels.row(count) = tet_voxels.row(i);
            count++;
        }
    }
    m_vertices = tet_vertices;
    m_voxels = m_voxels.topRows(count);
    m_voxels.col(0).swap(m_voxels.col(1));
}

#endif
