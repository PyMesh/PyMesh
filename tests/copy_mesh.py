#!/usr/bin/env python
import sys
sys.path.append("../lib/");
sys.path.append("../swig/");

import PyMesh

def main():
    filename="holey.obj"
    factory1 = PyMesh.MeshFactory();
    factory1.load_file(filename);
    mesh1 = factory1.create();


    vertices = mesh1.get_vertices();
    faces = mesh1.get_faces();
    voxels = mesh1.get_voxels();


    factory2 = PyMesh.MeshFactory();
    factory2.load_data(vertices, faces, voxels,
            mesh1.get_vertex_per_face(),
            mesh1.get_vertex_per_voxel());
    mesh2 = factory2.create();


    writer = PyMesh.MeshWriter.create_writer("tmp.obj");
    writer.write_mesh(mesh2);

if __name__ == "__main__":
    main();
