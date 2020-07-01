import numpy as np

from ..meshio import form_mesh
from PyMesh import LongEdgeRemoval

def split_long_edges_raw(vertices, faces, max_edge_length):
    """ Split long edges.

    Args:
        vertices (``numpy.ndarray``): Vertex array with one vertex per row.
        faces (``numpy.ndarray``): Face array with one face per row.
        max_edge_length (``float``): Maximum edge length allowed.  All edges
            longer than this will be split.

    Returns:
        3 values are returned.

            * ``output_vertices``: Output vertex array with one vertex per row.
            * ``output_faces``: Output face array with one face per row.
            * ``info``: Additional information dict.

        The following fiels are defined in the ``info`` dict:
            * ``ori_face_indices``: index array that maps output faces to their
              source input faces.
    """
    remover = LongEdgeRemoval(vertices, faces)
    remover.run(max_edge_length)
    vertices = remover.get_vertices()
    faces = remover.get_faces()
    return vertices, faces, {"ori_face_indices": remover.get_ori_faces()}

def split_long_edges(mesh, max_edge_length):
    """ Wrapper function of :func:`split_long_edges_raw`.

    Args:
        mesh (:class:`Mesh`): Input mesh.
        max_edge_length (``float``): Maximum edge length allowed.  All edges
            longer than this will be split.

    Returns:
        2 values are returned.

            * ``output_mesh`` (:class:`Mesh`): Output mesh.
            * ``info``: Additional information dictionary.
    """
    vertices, faces, info = split_long_edges_raw(
            mesh.vertices, mesh.faces, max_edge_length)
    return form_mesh(vertices, faces), info
