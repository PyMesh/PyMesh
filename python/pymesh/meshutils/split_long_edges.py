import numpy as np

from PyMeshUtils import LongEdgeRemoval

def split_long_edges(vertices, faces, max_edge_length):
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
            * ``information``: A dummy ``dict`` that is currently empty.
              It is here to ensure consistent interface across the module.
    """
    remover = LongEdgeRemoval(vertices, faces);
    remover.run(max_edge_length);
    vertices = remover.get_vertices();
    faces = remover.get_faces();
    return vertices, faces, {};
