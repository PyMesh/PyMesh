#include <iostream>
#include <string>
#include "EigenTypedef.h"
#include "Mesh.h"
#include "MeshFactory.h"
#include "MeshWriter.h"
#include "tetgen_wrapper.h"

void print_usage(char* exe_name) {
    std::cout << exe_name << " input_mesh output_mesh" << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        print_usage(argv[0]);
        return -1;
    }

    Mesh* in = MeshFactory().load_file(std::string(argv[1])).create();
    Mesh* out = NULL;

    VectorF& vertices = in->get_vertices();
    VectorI& faces = in->get_faces();

    //TetgenWrapper tetgen(*in);
    TetgenWrapper tetgen(vertices, faces, 3);
    std::string opt("qpa0.01");
    int err_code = tetgen.tetgen(opt);

    if (err_code != 0) {
        throw err_code;
    }

    VectorF tet_vertices = tetgen.get_vertices();
    VectorI tet_faces = tetgen.get_faces();
    VectorI tet_voxels = tetgen.get_voxels();

    const size_t num_vertex_per_face = 3;
    const size_t num_vertex_per_voxel = 4;

    MeshWriter* writer = MeshWriter::create_writer(std::string(argv[2]));
    writer->write(tet_vertices, tet_faces, tet_voxels, 
            num_vertex_per_face, num_vertex_per_voxel);
    return 0;
}

