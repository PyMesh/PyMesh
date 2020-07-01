import PyMesh

def straight_skeleton(vertices, edges):
    """ Compute 2D straight skeleton.

    Args:
        vertices (``numpy.ndarray``): Vertices of the input boundary.
        edges (``numpy.ndarray``): Edges of the input boundary.

    Returns:
        2 values are returned:

            * ``points``: Straight skeleton vertices.
            * ``segments``: Straight skeleton segments.
    """
    engine = PyMesh.StraightSkeleton()
    engine.run(vertices, edges)
    return engine.points, engine.segments
