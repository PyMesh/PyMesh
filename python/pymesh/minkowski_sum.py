import PyIGL
import numpy as np
from .meshio import form_mesh
from .meshutils import remove_isolated_vertices

def minkowski_sum(mesh, path):
    min_sum = PyIGL.MinkowskiSum.create_raw(mesh.vertices, mesh.faces);
    min_sum.run(path);

    result = form_mesh(min_sum.get_vertices(), min_sum.get_faces());

    return result;
