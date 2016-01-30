import PyIGL
import numpy as np
from .meshio import form_mesh
from .meshutils import remove_isolated_vertices

def minkowski_sum(mesh, path):
    """ Perform Minkowski sum of a mesh with a poly-line.

    Args:
        mesh (:class:`Mesh`): Input mesh.
        path (:class:`numpy.ndarray`): a :math:`n \times 3` matrix.  Each row
            represents a node in the poly-line.

    Returns: A mesh representing the Minkowski sum of the inputs.
    """
    min_sum = PyIGL.MinkowskiSum.create_raw(mesh.vertices, mesh.faces);
    min_sum.run(path);

    result = form_mesh(min_sum.get_vertices(), min_sum.get_faces());

    return result;
