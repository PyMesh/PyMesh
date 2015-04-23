from PyMeshUtils import IsolatedVertexRemoval

def remove_isolated_vertices(vertices, elements):
    """ Remove isolated vertices.

    Args:
        vertices (``numpy.ndarray``): Vertex array with one vertex per row.
        elements (``numpy.ndarray``): Element array with one face per row.

    Returns:
        3 values are returned.

            * ``output_vertices``: Output vertex array with one vertex per row.
            * ``output_elements``: Output element array with one element per row.
            * ``infomation``: A ``dict`` of additional informations.

        The following fields are defined in ``infomation``:

            * ``num_vertex_removed``: Number of vertex removed.
            * ``ori_vertex_index``: Original vertex index.  That is vertex ``i`` of
              ``output_vertices`` has index ``ori_vertex_index[i]`` in the input
              vertex array.
    """
    remover = IsolatedVertexRemoval(vertices, elements);
    num_removed = remover.run();
    vertices = remover.get_vertices();
    elements = remover.get_faces();
    info = {
            "num_vertex_removed": num_removed,
            "ori_vertex_index": remover.get_ori_vertex_indices().ravel(),
            };
    return vertices, elements, info;

