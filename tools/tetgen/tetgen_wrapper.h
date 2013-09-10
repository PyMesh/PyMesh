#pragma once
#include <string>
#include <tetgen.h>

#include "EigenTypedef.h"

class Mesh;

class TetgenWrapper {
    public:
        //TetgenWrapper(Mesh& surface);
        TetgenWrapper(const VectorF& vertices, const VectorI& faces,
                size_t num_vertex_per_face);

    public:
        void tetgen(const std::string& opt);

        VectorF get_vertices() { return m_tet_vertices; }
        VectorI get_faces() { return m_tet_faces; }
        VectorI get_voxels() { return m_tet_voxels; }

    private:
        void extract_vertices(tetgenio& mesh);
        void extract_faces(tetgenio& mesh);
        void extract_voxels(tetgenio& mesh);

        tetgenio create_tetgen_mesh(
                const VectorF& vertices, const VectorI& faces);

    private:
        VectorF m_ori_vertices;
        VectorI m_ori_faces;

        VectorF m_tet_vertices;
        VectorI m_tet_faces;
        VectorI m_tet_voxels;
};
