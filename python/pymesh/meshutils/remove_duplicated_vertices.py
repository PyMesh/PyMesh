from PyMeshUtils import DuplicatedVertexRemoval

def remove_duplicated_vertices(vertices, elements, tol=1e-12, importance=None):
    """ Merge duplicated vertices into a single vertex.

    Args:
        vertices (``numpy.ndarray``): Vertices in row major.
        elements (``numpy.ndarray``): Elements in row major.
        tol (``float``): (optional) Vertices with distance less than ``tol`` are
            considered as duplicates.  Default is ``1e-12``.
        importance (``numpy.ndarray``): (optional) Per-vertex importance value.
            When discarding duplicates, the vertex with the highest importance
            value will be kept.

    Returns:
        3 values are returned.

            * ``output_vertices``: Output vertices in row major.
            * ``output_elements``: Output elements in row major.
            * ``information``: A ``dict`` of additional informations.

        The following fields are defined in ``information``:

            * ``num_vertex_merged``: number of vertex merged.
            * ``index_map``: An array that maps input vertex index to output vertex index.
              I.e. vertex ``i`` will be mapped to ``index_map[i]`` in the output.

    """

    remover = DuplicatedVertexRemoval(vertices, elements);
    if importance is not None:
        if (len(importance) != len(vertices)):
            raise RuntimeError(
                    "Vertex importance must be of the same size as vertices");
        remover.set_importance_level(importance);
    num_merged = remover.run(tol);
    vertices = remover.get_vertices();
    elements = remover.get_faces();
    info = {
            "num_vertex_merged": num_merged,
            "index_map": remover.get_index_map().ravel(),
            };
    return vertices, elements, info;
