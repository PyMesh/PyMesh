#include <cstdlib>
#include <iostream>
#include <string>

#include <Core/EigenTypedef.h>
#include <IO/MeshWriter.h>
#include <Math/MatrixUtils.h>
#include <Wires/WireNetwork/WireNetwork.h>
#include <Wires/Inflator/InflatorEngine.h>

void print_usage(char* prog_name) {
    std::cout << "Usage: " << prog_name
        << " thickness"
        << " in_wire_file"
        <<" out_mesh_file"
        << std::endl;
}

void write_mesh(const std::string& filename,
        const MatrixFr& vertices, const MatrixIr& faces) {
    VectorI voxels = VectorI::Zero(0);
    MeshWriter::Ptr writer = MeshWriter::create(filename);
    writer->write(
            MatrixUtils::flatten<VectorF>(vertices),
            MatrixUtils::flatten<VectorI>(faces),
            voxels,
            vertices.cols(),
            faces.cols(),
            0);
}

int main(int argc, char** argv) {
    if (argc != 4) {
        print_usage(argv[0]);
        return 0;
    }

    Float thickness = atof(argv[1]);

    WireNetwork::Ptr wire_network = WireNetwork::create(argv[2]);
    InflatorEngine::Ptr inflator = InflatorEngine::create("simple", wire_network);
    inflator->set_uniform_thickness(thickness);
    inflator->inflate();

    MatrixFr vertices = inflator->get_vertices();
    MatrixIr faces = inflator->get_faces();

    write_mesh(argv[3], vertices, faces);

    return 0;
}
