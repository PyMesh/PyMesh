import math
import numpy as np
from ..meshio import form_mesh

def generate_regular_tetrahedron(edge_length=1.0, center=[0.0, 0.0, 0.0]):
    """ Generate a regular `tetrahedron`_ with the specified edge length and center.

    Args:
        edge_length (``float``): edge length of the regular tet.
        center (``numpy.ndarray``): center of the tet.

    Returns:
        A :py:class:`Mesh` object containing a regular tetrahedron.

    .. _tetrahedron: http://mathworld.wolfram.com/Tetrahedron.html
    """
    center = np.array(center)
    s = 1.0 / math.sqrt(2.0)

    vertices = np.array([
        [-1.0, 0.0, -s],
        [ 1.0, 0.0, -s],
        [ 0.0, 1.0,  s],
        [ 0.0, -1.0,  s] ])
    voxels = np.array([
        [0, 1, 2, 3]
        ])

    vertices = vertices * (0.5 * edge_length) + center
    return form_mesh(vertices, np.array([]), voxels)
