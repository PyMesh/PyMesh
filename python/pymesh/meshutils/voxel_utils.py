import PyMesh
import numpy as np

def get_tet_orientations_raw(vertices, tets):
    """ Compute orientation of each tet.

    Args:
        vertice (``numpy.ndarray``): n by 3 matrix representing vertices.
        tets (``numpy.ndarray``): m by 4 matrix of vertex indices representing tets.

    Returns:
        A list of m floats, where
            * Positive number => tet is positively oriented.
            *               0 => tet is degenerate.
            * Negative number => tet is inverted.
    """
    return PyMesh.get_tet_orientations(vertices, tets)

def get_tet_orientations(mesh):
    """ A thin wrapper of ``get_tet_orientations_raw``.
    """
    if mesh.num_voxels == 0:
        return np.zeros(0)
    if mesh.vertex_per_voxel != 4:
        raise NotImplementedError("Orientation computation expect a tet mesh.")
    return PyMesh.get_tet_orientations(mesh.vertices, mesh.voxels)

def is_delaunay_raw(vertices, tets):
    """ Compute whether each tet is strictly locally Delaunay, cospherical or
    not locally Delaunay.

    Args:
        vertices (``numpy.ndarray``): n by 3 matrix representing vertices.
        tets (``numpy.ndarray``): m by 4 matrix of vertex indices representing tets.

    Returns:
        A list of m floats representing result for each tet:
            * 1 => tet is strictly locally Delaunay.
            * 0 => tet is cospherical with an adjacent tet.
            * 1 => tet is not locally Delaunay.
    """
    return PyMesh.is_delaunay(vertices, tets)

def is_delaunay(mesh):
    """ A thin wrapper of ``is_delaunay_raw``.
    """
    if mesh.num_voxels == 0:
        return np.zeros(0)
    if mesh.vertex_per_voxel != 4:
        raise NotImplementedError("Delaunay property computation expect a tet mesh.")
    return PyMesh.is_delaunay(mesh.vertices, mesh.voxels)
