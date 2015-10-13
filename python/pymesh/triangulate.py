import PyTriangle
from .meshio import form_mesh

def triangulate(points, segments, max_area,
        holes=None,
        split_boundary=True,
        auto_hole_detection=False,
        use_steiner_points=True):
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
    assert(len(faces) > 0);
    assert(faces.shape[1] == 3);

    triangle_wrapper = PyTriangle.TriangleWrapper(vertices, faces);
    triangle_wrapper.run(max_area, split_boundary, False,
            use_steiner_points);

    vertices = triangle_wrapper.get_vertices();
    faces = triangle_wrapper.get_faces();

    return vertices, faces;

