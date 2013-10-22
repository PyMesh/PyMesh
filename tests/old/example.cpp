#include <iostream>
#include <Eigen/Core>
#include <string>

#include <Mesh.h>
#include <MeshFactory.h>
#include <MeshWriter.h>
#include <Timer.h>

int main(int argc, char* argv[]) {
    Timer timer("Example");
    MeshFactory factory;
    timer.tik("factory created");
    factory.load_file(argv[1]);
    timer.tik("file loaded");
    factory.with_connectivity("all");
    timer.tik("connectivity");
    factory.with_attribute("vertex_normal");
    timer.tik("normal");
    Mesh* mesh = factory.create();
    timer.tik("mesh created");
    //Mesh* mesh = MeshFactory()
    //    .load_file(argv[1])
    //    .with_connectivity()
    //    .with_attribute("vertex_normal")
    //    .create();
    timer.summary();

    VectorF& vertices = mesh->get_vertices();
    VectorI& faces = mesh->get_faces();
    VectorI& voxels = mesh->get_voxels();
    std::cout << "# vertices: " << vertices.size()/3 << std::endl;
    std::cout << "# faces: " << faces.size()/3 << std::endl;
    std::cout << "# voxels: " << voxels.size()/4 << std::endl;

    const VectorF& normals = mesh->get_attribute("vertex_normal");

    MeshWriter* writer = MeshWriter::create_writer("tmp.obj");
    //writer->write(*mesh);
    size_t vertex_per_face=3;
    size_t vertex_per_voxel=4;
    writer->write(vertices, faces, voxels, vertex_per_face, vertex_per_voxel);
    return 0;
}

