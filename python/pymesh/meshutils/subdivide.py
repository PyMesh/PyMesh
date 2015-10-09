import PyMeshUtils
from ..meshio import form_mesh

def subdivide(mesh, order=1, method="simple"):
    """ Subdivide the input mesh.

    Args:
        mesh: Input triangle mesh.
        order: (optional) Subdivision order.
        method: (optional) Subdivision method.  Choices are "simple" and "loop".

    Returns:
        Returns the subdivided mesh.  The per-face attribute "ori_face_index"
        tracks the original face index from the input mesh.

    """
    subdiv = PyMeshUtils.Subdivision.create(method);
    subdiv.subdivide(mesh.vertices, mesh.faces, order);

    vertices = subdiv.get_vertices();
    faces = subdiv.get_faces();
    ori_face_indices = subdiv.get_face_indices().ravel();

    subdivided_mesh = form_mesh(vertices, faces);
    subdivided_mesh.add_attribute("ori_face_index");
    subdivided_mesh.set_attribute("ori_face_index", ori_face_indices);
    return subdivided_mesh;
