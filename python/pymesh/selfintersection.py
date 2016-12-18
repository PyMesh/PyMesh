import numpy as np
import PySelfIntersection
from .meshio import form_mesh

def resolve_self_intersection(mesh, engine="auto"):
    """ Resolve all self-intersections.

    Args:
        mesh (:py:class:`Mesh`): The input mesh.  Only triangular mesh is
            supported.
        engine (``string``): (optional) Self-intersection engine.  Valid engines
            include:

            * ``auto``: the default engine (default is ``igl``).
            * ``igl``: `libigl's self-intersection engine
              <https://github.com/libigl/libigl>`_

    Returns:
        ``Mesh``: A triangular mesh with all self-intersection meshed.  The
        following per-face scalar field is defined:

        * ``face_sources``: For each output face, this field specifies the index
            of the corresponding "source face" in the input mesh.

    """
    if engine == "auto":
        engine = "igl";

    vertices = mesh.vertices;
    faces = mesh.faces;
    if mesh.dim == 2:
        vertices = np.hstack((vertices,
            np.zeros((mesh.num_vertices, 1), dtype=float)));
    elif mesh.dim != 3:
        raise NotImplementedError(
                "Resolving self-intersection only support 2D and 3D meshes");
    if mesh.vertex_per_face != 3:
        raise NotImplementedError(
                "Resolving self-intersection only support triangle meshes");

    resolver = PySelfIntersection.SelfIntersectionResolver.create(engine);
    resolver.set_mesh(vertices, faces);
    resolver.run();

    vertices = resolver.get_vertices();
    faces = resolver.get_faces();

    if mesh.dim == 2:
        vertices = vertices[:,[0,1]];

    output_mesh = form_mesh(vertices, faces);
    face_sources = resolver.get_face_sources();
    output_mesh.add_attribute("face_sources");
    output_mesh.set_attribute("face_sources", face_sources);

    return output_mesh;

def detect_self_intersection(mesh):
    """ Detect all self-intersections.

    Args:
        mesh (:class:`Mesh`): The input mesh.

    Returns:
        :py:class:`numpy.ndarray`: A n by 2 array of face indices.  Each
            row contains the indices of two intersecting faces. :math:`n` is
            the number of intersecting face pairs.
    """
    try:
        import PyCGAL
    except ImportError:
        raise NotImplementedError(
                "Self intersection detection is not supported");

    detector = PyCGAL.SelfIntersection(mesh.vertices, mesh.faces);
    detector.detect_self_intersection();
    intersecting_faces = detector.get_self_intersecting_pairs();
    return intersecting_faces;
