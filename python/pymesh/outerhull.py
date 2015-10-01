import PyOuterHull
import numpy as np
from .meshio import form_mesh
from .meshutils import remove_isolated_vertices_raw
from .meshutils import remove_degenerated_triangles_raw

def compute_outer_hull(mesh, engine="auto", all_layers=False):
    """ Compute the outer hull of the input mesh.

    Args:
        engine (:py:class:`str`): (optional) Outer hull engine name.  Valid engines are:

            * ``auto``: Using the default engine (``igl``).
            * ``igl``: `libigl's outer hull support
              <https://github.com/libigl/libigl>`_

        all_layers (:py:class:`bool`): (optional) If true, recursively peel
            outer hull layers.

    Returns:
        If ``all_layers`` is false, just return the outer hull mesh.

        If ``all_layers`` is ture, return a recursively peeled outer hull
            layers, from the outer most layer to the inner most layer.

        The following mesh attirbutes are defined in each outer hull mesh:

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
    flipped = engine.get_face_is_flipped().squeeze();
    ori_faces = engine.get_ori_face_indices().squeeze();
    layers = engine.get_outer_hull_layers().squeeze();
    to_flip = flipped != 0;
    faces[to_flip] = np.fliplr(faces[to_flip]);

    def extract_layer(i):
        selected_faces = layers == i;
        o_faces = faces[selected_faces];
        o_flipped = flipped[selected_faces];
        o_ori_faces = ori_faces[selected_faces];
        o_vertices, o_faces, __ = remove_isolated_vertices_raw(vertices, o_faces);
        o_vertices, o_faces, info = remove_degenerated_triangles_raw(
                o_vertices, o_faces);
        o_flipped = o_flipped[info["ori_face_indices"]];
        o_ori_faces = o_ori_faces[info["ori_face_indices"]];
        outer_hull = form_mesh(o_vertices, o_faces);
        outer_hull.add_attribute("flipped");
        outer_hull.set_attribute("flipped", o_flipped);
        outer_hull.add_attribute("face_sources");
        outer_hull.set_attribute("face_sources", o_ori_faces);
        return outer_hull;

    if not all_layers:
        result = extract_layer(0);
    else:
        num_layers = np.amax(layers) + 1;
        result = [extract_layer(i) for i in range(num_layers)];

    return result;

