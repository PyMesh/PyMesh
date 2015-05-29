import PyOuterHull
import numpy as np
from meshio import form_mesh


def compute_outer_hull(mesh, engine="auto", with_interior=False):
    """ Compute the outer hull of the input mesh.

    Args:
        engine (:py:class:`str`): (optional) Outer hull engine name.  Valid engines are:

            * ``auto``: Using the default engine (``igl``).
            * ``igl``: `libigl's outer hull support
              <https://github.com/libigl/libigl>`_

        with_interior (:py:class:`bool`): (optional) If true, return the interior
            mesh too.

    Returns:
        If ``with_interior`` is false, just return the outer hull mesh.

        If ``with_interior`` is ture, return the tuple
        ``(outer_hull_mesh, interior_mesh)``.

        The following mesh attirbutes are defined in the outer hull mesh:

        * ``flipped``: A per-face attribute that is true if a face in outer
          hull is orientated differently comparing to its corresponding face
          in the input mesh.
        * ``face_sources``: A per-face attribute that specifies the index of the
          source face in the input mesh.
    """
    assert(mesh.dim == 3);
    assert(mesh.vertex_per_face == 3);

    if engine == "auto":
        engine = "igl";

    engine = PyOuterHull.OuterHullEngine.create(engine);
    engine.set_mesh(mesh.vertices, mesh.faces);
    engine.run();

    vertices = engine.get_vertices();
    faces = engine.get_faces();
    flipped = engine.get_face_is_flipped();
    ori_faces = engine.get_ori_face_indices();

    outer_hull_mesh = form_mesh(vertices, faces);
    outer_hull_mesh.add_attribute("flipped");
    outer_hull_mesh.set_attribute("flipped", flipped);
    outer_hull_mesh.add_attribute("face_sources");
    outer_hull_mesh.set_attribute("face_sources", ori_faces);

    if with_interior:
        interior_vertices = engine.get_interior_vertices();
        interior_faces = engine.get_interior_faces();

        interior_mesh = form_mesh(interior_vertices, interior_faces);

        return outer_hull_mesh, interior_mesh;
    else:
        return outer_hull_mesh;

