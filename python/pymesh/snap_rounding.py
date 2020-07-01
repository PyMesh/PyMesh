import PyMesh
import numpy as np
from numpy.linalg import norm
from .meshutils import remove_duplicated_vertices_raw

def snap_rounding(points, segments, pixel_size, use_iterative=True):
    """ 2D snap rounding.

    Args:
        points (``numpy.ndarray``): Input points.
        segments (``numpy.ndarray``): Input segments.
        pixel_size (``float``): Pixel size.
        use_iterative (``bool``): Whether to use iterative snap rounding.

    Returns:
        2 values are returned.

            * ``vertices``: Snap rounded vertices.
            * ``edges``: Edge connecting vertices.
    """
    engine = PyMesh.SnapRounding2()
    engine.points = points
    engine.segments = segments
    engine.run(pixel_size, use_iterative)

    vertices = engine.vertices
    edges = engine.edges

    vertices, edges, __ = remove_duplicated_vertices_raw(
            vertices, edges, tol=pixel_size/2)
    return vertices, edges

