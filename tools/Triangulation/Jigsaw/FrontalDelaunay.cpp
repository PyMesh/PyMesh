/* This file is part of PyMesh. Copyright (c) 2018 by Qingnan Zhou */
#ifdef WITH_JIGSAW

#include "FrontalDelaunay.h"
extern "C" {
#include <lib_jigsaw.h>
#include <jigsaw_msh_t.h>
#include <jigsaw_jig_t.h>
}

using namespace PyMesh;

void Jigsaw::FrontalDelaunay::run() {
    jigsaw_msh_t input_mesh;
    jigsaw_jig_t input_jig;
    jigsaw_init_msh_t(&input_mesh);
    jigsaw_init_jig_t(&input_jig);
    input_mesh._flags = JIGSAW_EUCLIDEAN_MESH;
    input_jig._verbosity = 1;
    input_jig._mesh_dims = 2;
    input_jig._mesh_kern = JIGSAW_KERN_DELFRONT;
    input_jig._geom_feat = true;
    //input_jig._mesh_top1 = true;

    const size_t num_pts = m_points.rows();
    jigsaw_alloc_vert2(&input_mesh._vert2, num_pts);
    for (size_t i=0; i<num_pts; i++) {
        input_mesh._vert2._data[i]._ppos[0] = m_points(i,0);
        input_mesh._vert2._data[i]._ppos[1] = m_points(i,1);
        // Not sure what does itag do...
        input_mesh._vert2._data[i]._itag = i;
    }

    const size_t num_segments = m_segments.rows();
    jigsaw_alloc_edge2(&input_mesh._edge2, num_segments);
    for (size_t i=0; i<num_segments; i++) {
        input_mesh._edge2._data[i]._node[0] = m_segments(i, 0);
        input_mesh._edge2._data[i]._node[1] = m_segments(i, 1);
        // Again, not sure what does itag do...
        input_mesh._edge2._data[i]._itag = i;
    }

    jigsaw_alloc_bound(&input_mesh._bound, 4);
    input_mesh._bound._data[0]._itag = 0;
    input_mesh._bound._data[0]._indx = 0;
    input_mesh._bound._data[0]._kind = JIGSAW_EDGE2_TAG;
    input_mesh._bound._data[1]._itag = 0;
    input_mesh._bound._data[1]._indx = 1;
    input_mesh._bound._data[1]._kind = JIGSAW_EDGE2_TAG;
    input_mesh._bound._data[2]._itag = 0;
    input_mesh._bound._data[2]._indx = 2;
    input_mesh._bound._data[2]._kind = JIGSAW_EDGE2_TAG;
    input_mesh._bound._data[3]._itag = 0;
    input_mesh._bound._data[3]._indx = 3;
    input_mesh._bound._data[3]._kind = JIGSAW_EDGE2_TAG;

    jigsaw_msh_t output_mesh;
    auto status = jigsaw_make_mesh(
            &input_jig,
            &input_mesh,
            NULL, // Initial triangle mesh
            NULL, // Mesh spacing function
            &output_mesh);
    if (status) {
        jigsaw_free_msh_t(&input_mesh);
        throw RuntimeError("Jigsaw failed with status: "
                + std::to_string(status));
    }

    const size_t num_vertices = output_mesh._vert2._size;
    m_vertices.resize(num_vertices, 2);
    for (size_t i=0; i<num_vertices; i++) {
        m_vertices(i, 0) = output_mesh._vert2._data[i]._ppos[0];
        m_vertices(i, 1) = output_mesh._vert2._data[i]._ppos[1];
    }

    const size_t num_faces = output_mesh._tria3._size;
    m_faces.resize(num_faces, 3);
    for (size_t i=0; i<num_faces; i++) {
        m_faces(i, 0) = output_mesh._tria3._data[i]._node[0];
        m_faces(i, 1) = output_mesh._tria3._data[i]._node[1];
        m_faces(i, 2) = output_mesh._tria3._data[i]._node[2];
    }

    assert(output_mesh._tria4._size == 0);
    assert(output_mesh._value._size == 0);

    jigsaw_free_msh_t(&input_mesh);
    jigsaw_free_msh_t(&output_mesh);
}

#endif
