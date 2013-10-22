#!/usr/bin/env python
import sys
sys.path.append("../lib/");
sys.path.append("../swig/");

import PyMesh

def main():
    print("Start!");
    filename = "holey.obj"
    mesh = PyMesh.MeshFactory.create_mesh(filename);
    vertices = mesh.get_vertices();
    faces = mesh.get_faces();
    voxels = mesh.get_voxels();
    print("# vertices: {}".format(len(vertices)/3));
    print("# faces   : {}".format(len(faces)/3));
    print("# voxels  : {}".format(len(voxels)/4));

if __name__ == "__main__":
    main();
