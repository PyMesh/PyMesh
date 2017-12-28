import PyMesh
import numpy as np

def get_tet_orientations_raw(vertices, tets):
    return PyMesh.get_tet_orientations(vertices, tets);

def get_tet_orientations(mesh):
    return PyMesh.get_tet_orientations(mesh.vertices, mesh.voxels);
