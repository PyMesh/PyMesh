import PyMesh
import numpy as np
from time import time
from .meshio import form_mesh


def statistics(mesh, operation, engine="auto", with_timing=False, exact_mesh_file=None):
    """ Computes statistics of the input meshes.

    Args:
        mesh (:class:`Mesh`): The input mesh, :math:`M_1`.
        operation (``string``): The name of the operation.  Valid choices are:


        engine (``string``): (optional) Statistics engine name.  Valid engines include:

            * ``auto``: Using the default boolean engine
              (``cgal`` by default).  This is the default.
            * ``cgal``: `CGAL 3D boolean operations on Nef Polyhedra
              <http://doc.cgal.org/latest/Nef_3/index.html>`_

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
    assert(mesh.vertex_per_face == 3)

    if engine == "auto":
        engine = "cgal"

    engine = PyMesh.BooleanEngine.create(engine)
    engine.set_mesh(mesh.vertices, mesh.faces)

    if with_timing:
        start_time = time()

    if operation == "intersection":
        engine.compute_intersection()
    elif operation == "union":
        engine.compute_union()
    elif operation == "difference":
        engine.compute_difference()
    elif operation == "symmetric_difference":
        engine.compute_symmetric_difference()
    else:
        raise NotImplementedError(
                "Unsupported operations: {}".format(operation))

    if with_timing:
        finish_time = time()
        running_time = finish_time - start_time

    output_mesh = form_mesh(engine.get_vertices(), engine.get_faces())
    face_sources = engine.get_face_sources()
    if len(face_sources) != 0:
        output_mesh.add_attribute("source_face")
        output_mesh.set_attribute("source_face", face_sources)
        output_mesh.add_attribute("source")
        sources = face_sources < mesh_1.num_faces
        output_mesh.set_attribute("source", sources)

    if exact_mesh_file is not None:
        engine.serialize_xml(exact_mesh_file)

    if with_timing:
        return output_mesh, running_time
    else:
        return output_mesh

