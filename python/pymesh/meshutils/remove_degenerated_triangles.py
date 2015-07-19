from PyMeshUtils import DegeneratedTriangleRemoval
from ..meshio import form_mesh

def remove_degenerated_triangles_raw(vertices, faces, num_iterations=5):
    """ Remove degenerated triangles.

    Degenerated faces are faces with zero area.  It is impossible to compute
    face normal for them.  This method get rid of all degenerated faces.  No new
    vertices will be introduced.  Only connectivity is changed.

    Args:
        vertices (``numpy.ndarray``): Vertex array with one vertex per row.
        faces (``numpy.ndarray``): Face array with one triangle per row.

    Returns:
        2 values are returned.

            * ``output_vertices``: Output vertex array, one vertex per row.
            * ``output_faces``: Output face array, one face per row.
    """
    if (faces.shape[1] != 3):
        raise RuntimeError("Faces are not triangles!");
    remover = DegeneratedTriangleRemoval(vertices, faces);
    remover.run(num_iterations);
    return remover.get_vertices(), remover.get_faces();

def remove_degenerated_triangles(mesh, num_iterations=5):
    """ Wrapper function of :func:`remove_degenerated_triangles_raw`.

    Args:
        mesh (:class:`Mesh`): Input mesh.

    Returns:
        ``Mesh``: Output mesh without degenerated triangles.
    """
    vertices, faces = remove_degenerated_triangles_raw(
            mesh.vertices, mesh.faces);
    return form_mesh(vertices, faces);
