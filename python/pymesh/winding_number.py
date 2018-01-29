import PyMesh

def compute_winding_number(mesh, queries, engine="auto"):
    """ Compute winding number with respect to `mesh` at `queries`.

    Args:
        mesh (:class:`Mesh`): The mesh for which winding number is evaluated.
        queries (:class:`numpy.ndarray`): N by 3 matrix of query points at which
            winding number is evaluated.

    Returns:
        A list of size N, represent the winding nubmers at each query points in
        order.
    """
    assert(mesh.dim == 3);
    assert(mesh.vertex_per_face == 3);

    if engine == "auto":
        engine = "igl";

    engine = PyMesh.WindingNumberEngine.create(engine);
    engine.set_mesh(mesh.vertices, mesh.faces);
    winding_numbers = engine.run(queries).ravel();

    return winding_numbers;
