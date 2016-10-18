import PyTriangle
import PyTriangulation
from .meshio import form_mesh

def triangulate_beta(points, engine="auto"):
    if engine == "auto":
        engine = "igl_lexicographic";

    engine = PyTriangulation.Triangulation.create(engine);
    engine.set_vertices(points);
    engine.run();
    faces = engine.get_faces();
    return form_mesh(points, faces);

def triangulate(points, segments, max_area,
        holes=None,
        split_boundary=True,
        auto_hole_detection=False,
        use_steiner_points=True):
    """ 2D and 3D planar Delaunay triangulation.

    Args:
        points (:class:`numpy.ndarray`): A set of 2D or 3D planar nodes.
        segments (:class:`numpy.ndarray`): Segments that connect nodes.
        max_area (float): The desired maximum output triangle area.
        holes (:class:`numpy.ndarray`): (optional) A set of 2D or 3D points
            such that the region containing any of these points are treated
            as holes.  If set to None, assumes no hole exists unless
            ``auto_hole_detection`` flag is set.
        split_boundary (bool): (optional) Whether splitting boundary segment is
            allowed.
        auto_hole_detection (bool): (optional) Automatically determine the
            regions that correspond to holes.
        use_steiner_points (bool): (optional) Whether to allow insertion of
            steiner points (automatically generagted points added to the
            interior of the mesh to improve triangle quality).

    Note that ``points`` and ``segments`` should form one or more closed
    planar curve representing the boundaries of the 2D regions to be
    triangulated.

    Returns:
        The output ``vertex`` and ``faces``.
    """
    triangle_wrapper = PyTriangle.TriangleWrapper(
            points, segments);
    if (holes is not None):
        triangle_wrapper.set_holes(holes);
    triangle_wrapper.run(max_area, split_boundary, auto_hole_detection,
            use_steiner_points);

    vertices = triangle_wrapper.get_vertices();
    faces = triangle_wrapper.get_faces();

    return vertices, faces;

def retriangulate(mesh, max_area,
        split_boundary=True,
        use_steiner_points=True):
    """ Retriangulate a given 2D or 3D planar mesh.

    Args:
        mesh (:py:class:`Mesh`): The input 2D or 3D planar mesh.
        max_area (float): The desired maximum output triangle area.
        split_boundary (bool): (optional) Whether splitting boundary segment is
            allowed.
        use_steiner_points (bool): (optional) Whether to allow insertion of
            steiner points (automatically generagted points added to the
            interior of the mesh to improve triangle quality).

    Returns:
        The retriangulated mesh.
    """
    assert(mesh.num_faces > 0);
    assert(mesh.vertex_per_face == 3);

    triangle_wrapper = PyTriangle.TriangleWrapper(
            mesh.vertices, mesh.faces);
    triangle_wrapper.run(max_area, split_boundary, False,
            use_steiner_points);

    vertices = triangle_wrapper.get_vertices();
    faces = triangle_wrapper.get_faces();

    return form_mesh(vertices, faces);

def retriangulate_raw(vertices, faces, max_area,
        split_boundary=True,
        use_steiner_points=True):
    """ Retriangulate a given 2D or 3D planar mesh.

    Args:
        vertices (:py:class:`numpy.ndarray`): The vertices of the input mesh.
        faces (:py:class:`numpy.ndarray`): The faces of the input mesh.
        max_area (float): The desired maximum output triangle area.
        split_boundary (bool): (optional) Whether splitting boundary segment is
            allowed.
        use_steiner_points (bool): (optional) Whether to allow insertion of
            steiner points (automatically generagted points added to the
            interior of the mesh to improve triangle quality).

    Returns:
        The ``vertices`` and ``faces`` of retriangulated mesh.
    """
    assert(len(faces) > 0);
    assert(faces.shape[1] == 3);

    triangle_wrapper = PyTriangle.TriangleWrapper(vertices, faces);
    triangle_wrapper.run(max_area, split_boundary, False,
            use_steiner_points);

    vertices = triangle_wrapper.get_vertices();
    faces = triangle_wrapper.get_faces();

    return vertices, faces;

