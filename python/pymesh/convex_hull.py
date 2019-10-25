import PyMesh

import numpy as np
from .meshio import form_mesh
from .triangle import triangle

def convex_hull(mesh, engine="auto", with_timing=False):
    """
    Compute the convex hull of an input mesh.

    Args:
        mesh (:class:`Mesh`): The input mesh.
        engine (``string``): (optional) Convex hull engine name.  Valid names are:

            * `auto`: Using the default engine.
            * `qhull`: `Qhull convext hull library <http://www.qhull.org/>`_
            * `cgal`: CGAL convex hull module (
                `2D <https://doc.cgal.org/latest/Convex_hull_2/index.html>`_,
                `3D <https://doc.cgal.org/latest/Convex_hull_3/index.html>`_)
            * `triangle`: Triangle convex hull engine.
            * `tetgen`: Tetgen convex hull engine.
        with_timing (``boolean``): (optional) Whether to time the code

    Returns: The output mesh representing the convex hull.
    (and running time if `with_timing` is true.)

    The following attributes are defined in the output mesh:

        * "source_vertex": An array of source vertex indices into the input mesh.
    """

    if engine == "auto":
        engine = "qhull"

    engine = PyMesh.ConvexHullEngine.create(mesh.dim, engine)

    if with_timing:
        start_time = time()

    engine.run(mesh.vertices)

    if with_timing:
        finish_time = time()
        running_time = finish_time - start_time

    vertices = engine.get_vertices()
    faces = engine.get_faces()
    index_map = engine.get_index_map()

    if mesh.dim == 3:
        convex_hull = form_mesh(vertices, faces)
        convex_hull.add_attribute("source_vertex")
        convex_hull.set_attribute("source_vertex", index_map)
    elif mesh.dim == 2:
        tri = triangle()
        tri.verbosity = 0
        tri.points = vertices
        tri.segments = faces
        tri.split_boundary = True
        tri.max_num_steiner_points = 0
        tri.run()
        convex_hull = tri.mesh
        convex_hull.add_attribute("source_vertex")
        convex_hull.set_attribute("source_vertex", index_map)
    else:
        raise NotImplementedError("Unsupported dimension: {}".format(mesh.dim))

    if with_timing:
        return convex_hull, running_time
    else:
        return convex_hull
