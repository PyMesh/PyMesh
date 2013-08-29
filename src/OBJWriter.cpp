#include "OBJWriter.h"

#include <iostream>
#include <fstream>
#include <cassert>

#include "EigenTypedef.h"
#include "Exception.h"

MeshWriter& OBJWriter::with_attribute(const std::string& attr_name) {
    std::cerr << "Error: .obj format does not support attributes." << std::endl;
    return *this;
}

void OBJWriter::write_mesh(Mesh& mesh) {
    write(mesh.get_vertices(), mesh.get_faces(), mesh.get_voxels(),
            mesh.get_vertex_per_face(), mesh.get_vertex_per_voxel());
}

void OBJWriter::write(VectorF& vertices, VectorI& faces, VectorI& voxels,
        size_t vertex_per_face, size_t vertex_per_voxel) {
    if (vertex_per_face != 3) {
        std::cerr << "Error: non-triangle face with "
            << vertex_per_face << " vertices." << std::endl;
        throw RuntimeError("Unsupported face type.");
    }

    std::ofstream fout(m_filename.c_str());
    fout << "# Generated with PyMesh" << std::endl;

    size_t num_vertices = vertices.size() / 3;
    for (size_t i=0; i<num_vertices; i++) {
        Vector3F v = vertices.segment<3>(i*3);
        fout << "v " << v[0] << " " << v[1] << " " << v[2] << std::endl;
    }

    size_t num_faces = faces.size() / 3;
    for (size_t i=0; i<num_faces; i++) {
        Vector3I f = faces.segment<3>(i*3);
        fout << "f " << f[0]+1 << " " << f[1]+1 << " " << f[2]+1 << std::endl;
    }
    fout.close();
}
