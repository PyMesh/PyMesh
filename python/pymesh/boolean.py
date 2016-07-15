import PyBoolean
import numpy as np
from time import time
from .meshio import form_mesh

from . import boolean_unsupported

def _auto_select_engine(dim):
    if dim == 2:
        engine = "clipper";
    elif dim == 3:
        engine = "igl";
    else:
        raise NotImplementedError("Dimension {} is not supported".format(dim));
    return engine;

def boolean(mesh_1, mesh_2, operation, engine="auto", with_timing=False,
        exact_mesh_file=None):
    """ Perform boolean operations on input meshes.

    Args:
        mesh_1 (:class:`Mesh`): The first input mesh, :math:`M_1`.
        mesh_2 (:class:`Mesh`): The second input mesh, :math:`M_2`.
        operation (``string``): The name of the operation.  Valid choices are:

            * ``intersection``: :math:`M_1 \cap M_2`
            * ``union``: :math:`M_1 \cup M_2`
            * ``difference``: :math:`M_1 \setminus M_2`
            * ``symmetric_difference``: :math:`(M_1 \setminus M_2) \cup
              (M_2 \setminus M_1)`

        engine (``string``): (optional) Boolean engine name.  Valid engines include:

            * ``auto``: Using the default boolean engine
              (``igl`` for 3D and ``clipper`` for 2D).  This is the default.
            * ``cork``: `Cork 3D boolean libary
              <https://github.com/gilbo/cork>`_
            * ``cgal``: `CGAL 3D boolean operations on Nef Polyhedra
              <http://doc.cgal.org/latest/Nef_3/index.html>`_
            * ``corefinement``: The undocumented CGAL boolean function that does
              not use Nef Polyhedra.
            * ``igl``: `libigl's 3D boolean support
              <https://github.com/libigl/libigl>`_
            * ``clipper``: `Clipper 2D boolean library
              <http://www.angusj.com/delphi/clipper.php>`_
            * ``carve``: `Carve solid geometry library
              <https://code.google.com/p/carve/>`_

        with_timing (``boolean``): (optional) Whether to time the code.
        exact_mesh_file (``str``): (optional) Filename to store the XML
            serialized exact output.

    Returns: The output mesh.

    The following attributes are defined in the output mesh:

        * "source": An array of 0s and 1s indicating which input mesh an output
          face comes from.
        * "source_face": An array of indices, one per output face, into the
          concatenated faces of the input meshes.
    """
    assert(mesh_1.dim == mesh_2.dim);
    assert(mesh_1.vertex_per_face == 3);
    assert(mesh_2.vertex_per_face == 3);
    dim = mesh_1.dim;

    if engine == "auto":
        engine = _auto_select_engine(dim);
    elif engine == "quick_csg":
        return boolean_unsupported.quick_csg(mesh_1, mesh_2, operation,
                with_timing);

    engine = PyBoolean.BooleanEngine.create(engine);
    engine.set_mesh_1(mesh_1.vertices, mesh_1.faces);
    engine.set_mesh_2(mesh_2.vertices, mesh_2.faces);

    if with_timing:
        start_time = time();

    if (operation == "intersection"):
        engine.compute_intersection();
    elif (operation == "union"):
        engine.compute_union();
    elif (operation == "difference"):
        engine.compute_difference();
    elif (operation == "symmetric_difference"):
        engine.compute_symmetric_difference();
    else:
        raise NotImplementedError(
                "Unsupported operations: {}".format(operation));

    if with_timing:
        finish_time = time();
        running_time = finish_time - start_time;

    output_mesh = form_mesh(engine.get_vertices(), engine.get_faces());
    face_sources = engine.get_face_sources();
    if len(face_sources) != 0:
        output_mesh.add_attribute("source_face");
        output_mesh.set_attribute("source_face", face_sources);
        output_mesh.add_attribute("source");
        sources = face_sources < mesh_1.num_faces;
        output_mesh.set_attribute("source", sources);

    if exact_mesh_file is not None:
        engine.serialize_xml(exact_mesh_file);

    if with_timing:
        return output_mesh, running_time;
    else:
        return output_mesh;

