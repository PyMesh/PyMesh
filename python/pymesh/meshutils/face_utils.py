import PyMeshUtils
import numpy as np

def is_colinear(v0, v1, v2):
    """ Return true if ``v0``, ``v1`` and ``v2`` are colinear.
    Colinear check is done using exact predicates.

    Args:
        v0 (``numpy.ndarray``): vector of size 2 or 3.
        v1 (``numpy.ndarray``): vector of size 2 or 3.
        v2 (``numpy.ndarray``): vector of size 2 or 3.

    Return:
        bool: whether ``v0``, ``v1`` and ``v2`` are colinear.
    """
    dim = len(v0);
    if dim == 2:
        return PyMeshUtils.is_colinear_2D(v0, v1, v2);
    elif dim == 3:
        return PyMeshUtils.is_colinear_3D(v0, v1, v2);
    else:
        raise NotImplementedError("Supported dimention {}".format(dim));

def get_degenerated_faces_raw(vertices, faces):
    """ Return indices of degenerated faces.
    A face is degenerated if all its 3 corners are colinear.

    Args:
        vertices (``numpy.ndarray``): Vertex matrix.
        faces (``numpy.ndarray``): Face matrix.

    Returns:
        ``numpy.ndarray``: List of indices of degenerated faces.
    """
    return np.array(PyMeshUtils.get_degenerated_faces(vertices, faces));

def get_degenerated_faces(mesh):
    """ A thin wrapper for :py:func:`get_degenerated_faces_raw`.
    """
    return get_degenerated_faces_raw(mesh.vertices, mesh.faces);
