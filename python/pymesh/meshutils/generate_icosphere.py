import math
import numpy as np
from numpy.linalg import norm

from ..meshio import form_mesh
from .subdivide import subdivide

def generate_icosphere(radius, center, refinement_order=0):
    """ Generate icosphere.

    Args:
        radius (``float``): Radius of icosphere.
        center (``numpy.ndarray``): Sphere center.
        refinement_order (``int``): (optional) Number of refinement.

    Returns:
        The icosphere mesh.
    """
    r = (1.0 + math.sqrt(5.0)) / 2.0;
    vertices = np.array([
        [-1.0,   r, 0.0],
        [ 1.0,   r, 0.0],
        [-1.0,  -r, 0.0],
        [ 1.0,  -r, 0.0],
        [0.0, -1.0,   r],
        [0.0,  1.0,   r],
        [0.0, -1.0,  -r],
        [0.0,  1.0,  -r],
        [  r, 0.0, -1.0],
        [  r, 0.0,  1.0],
        [ -r, 0.0, -1.0],
        [ -r, 0.0,  1.0],
        ], dtype=float);

    faces = np.array([
        [0, 11, 5],
        [0, 5, 1],
        [0, 1, 7],
        [0, 7, 10],
        [0, 10, 11],
        [1, 5, 9],
        [5, 11, 4],
        [11, 10, 2],
        [10, 7, 6],
        [7, 1, 8],
        [3, 9, 4],
        [3, 4, 2],
        [3, 2, 6],
        [3, 6, 8],
        [3, 8, 9],
        [5, 4, 9],
        [2, 4, 11],
        [6, 2, 10],
        [8, 6, 7],
        [9, 8, 1],
        ]);

    mesh = form_mesh(vertices, faces);
    mesh = subdivide(mesh, refinement_order);

    length = norm(mesh.vertices, axis=1).reshape((-1, 1));
    vertices = mesh.vertices / length * radius + center;

    return form_mesh(vertices, mesh.faces);
