from math import radians

from .. import timethis
from ..meshio import form_mesh
from PyMeshUtils import ObtuseTriangleRemoval

class ObtuseTriangleRemover:
    def __init__(self, vertices, faces):
        if vertices.shape[1] != 3:
            raise RuntimeError("Only 3D meshes are supported");
        if faces.shape[1] != 3:
            raise RuntimeError("Only triangular meshes are supported");

        self.vertices = vertices;
        self.faces = faces;

    def remove_obtuse_triangles(self, max_angle, max_num_iterations):
        max_angle = radians(max_angle);
        remover = ObtuseTriangleRemoval(self.vertices, self.faces);
        num_triangles_split = remover.run(
                max_angle, max_num_iterations);
        self.vertices = remover.get_vertices();
        self.faces = remover.get_faces();
        return num_triangles_split;

def remove_obtuse_triangles_raw(vertices, faces,
        max_angle=120,
        max_iterations=5):
    """ Remove all obtuse triangles.

    Args:
        vetices (``numpy.ndarray``): Vertex array with one vertex per row.
        faces (``numpy.ndarray``): Face array with one face per row.
        max_angle (``float``): (optional) Maximum obtuse angle in degrees
            allowed.  All triangle with larger internal angle would be split.
            Default is 120 degrees.
        max_iterations (``int``): (optional) Number of iterations to run before
            quitting.  Default is 5.

    Returns:
        3 values are returned.

            * ``output_vertices``: Output vertex array with one vertex per row.
            * ``output_faces``: Output face array with one face per row.
            * ``information``: A ``dict`` of additinal informations.

        The following fields are defiend in ``information``:

            * ``num_triangle_split``: number of triangles split.
    """
    if max_angle < 90:
        raise RuntimeError("max_angle must be greater than 90 degrees");

    remover = ObtuseTriangleRemover(vertices, faces);
    num_split = remover.remove_obtuse_triangles(max_angle, max_iterations);
    info = {
            "num_triangle_split": num_split
            };
    return remover.vertices, remover.faces, info;

def remove_obtuse_triangles(mesh, max_angle=120, max_iterations=5):
    """ Wrapper function of :func:`remove_obtuse_triangles_raw`.

    Args:
        mesh (:class:`Mesh`): Input mesh.
        max_angle (``float``): (optional) Maximum obtuse angle in degrees
            allowed.  All triangle with larger internal angle would be split.
            Default is 120 degrees.
        max_iterations (``int``): (optional) Number of iterations to run before
            quitting.  Default is 5.

    Returns:
        2 values are returned.

            * ``output_mesh`` (:class:`Mesh`): Output mesh.
            * ``information`` (:class:`dict`): A ``dict`` of additinal informations.

        The following fields are defiend in ``information``:

            * ``num_triangle_split``: number of triangles split.
    """
    vertices, faces, info = remove_obtuse_triangles_raw(
            mesh.vertices, mesh.faces, max_angle, max_iterations);
    return form_mesh(vertices, faces), info;
