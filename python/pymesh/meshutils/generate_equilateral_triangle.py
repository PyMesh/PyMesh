import math
import numpy as np
import math
from ..meshio import form_mesh

def generate_equilateral_triangle(edge_length=1.0, center=[0.0, 0.0]):
    """ Generate a equilateral 2D triangle with the specified edge length and center.

    Args:
        edge_length (``float``): edge length of the triangle.
        center (``numpy.ndarray``): center of the triangle.

    Returns:
        A :py:class:`Mesh` object containing a equilateral triangle.
    """
    center = np.array(center)
    sqrt_3 = math.sqrt(3.0)
    vertices = np.array([
        [-0.5, -sqrt_3 / 6.0],
        [ 0.5, -sqrt_3 / 6.0],
        [ 0.0, sqrt_3 / 3.0],
        ]) * edge_length - center
    faces = np.array([[0,1,2]])

    return form_mesh(vertices, faces)

