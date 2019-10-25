import PyMesh

def compute_winding_number(mesh, queries, engine="auto"):
    """ Compute winding number with respect to `mesh` at `queries`.

    Args:
        mesh (:class:`Mesh`): The mesh for which winding number is evaluated.
        queries (:class:`numpy.ndarray`): N by 3 matrix of query points at which
            winding number is evaluated.
        engine (``string``): (optional) Winding number computing engine name:

            * ``auto``: use default engine (which is ``igl``).
            * ``igl``: use libigl's `generalized winding number`_.
            * ``fast_winding_number``: use code from `fast winding number`_
              paper. It is faster than ``igl`` but can be less accurate sometimes.


    Returns:
        A list of size N, represent the winding numbers at each query points in
        order.

    .. _`generalized winding number`: https://libigl.github.io/tutorial/#generalized-winding-number
    .. _`fast winding number`: http://www.dgp.toronto.edu/projects/fast-winding-numbers/
    """
    assert(mesh.dim == 3)
    assert(mesh.vertex_per_face == 3)

    if engine == "auto":
        engine = "igl"

    engine = PyMesh.WindingNumberEngine.create(engine)
    engine.set_mesh(mesh.vertices, mesh.faces)
    winding_numbers = engine.run(queries).ravel()

    return winding_numbers
