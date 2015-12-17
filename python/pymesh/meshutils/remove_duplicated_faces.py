import numpy as np
from PyMeshUtils import FinFaceRemoval
from ..meshio import form_mesh

def remove_duplicated_faces_raw(vertices, faces, fins_only=False):
    """ Remove duplicated faces.

    Duplicated faces are defined as faces consist of the same set of vertices.
    Depending on the face orientation.  A special case of duplicated faces is
    a fin.  A fin is defined as two duplicated faces with opposite orientaiton.

    If fins_only is set to True, all fins in the mesh are removed.  The output
    mesh could still contain duplicated faces but no fins.

    If fins_only is not True, all duplicated faces will be removed.  There could
    be two caes:

    If there is a dominant orientation, that is more than half of the faces are
    consistently orientated, and fins_only is False, one face with the dominant
    orientation will be kept while all other faces are removed.

    If there is no dominant orientation, i.e. half of the face are positively
    orientated and the other half is negatively orientated, all faces are
    discarded.

    Args:
        vertices (``numpy.ndarray``): Vertex array with one vertex per row.
        faces (``numpy.ndarray``): Face array with one face per row.
        fins_only (``bool``): If set, only remove fins.

    Returns:
        3 values are returned.

            * ``output_vertices``: Output vertex array, one vertex per row.
            * ``output_faces``: Output face array, one face per row.
            * ``information``: A ``dict`` of additional informations.

        The following fields are defined in ``information``:

            * ``ori_face_index``: An array of original face indices. I.e. face
              ``i`` of the ``output_faces`` has index ``ori_face_index[i]`` in
              the input vertices.

    """
    remover = FinFaceRemoval(vertices, faces);
    if fins_only: remover.set_fins_only();
    remover.run();
    info = {
            "ori_face_index": remover.get_face_indices().ravel(),
            };
    return remover.get_vertices(), remover.get_faces(), info;

def remove_duplicated_faces(mesh, fins_only=False):
    """ Wrapper function of :func:`remove_duplicated_faces_raw`.

    Args:
        mesh (:class:`Mesh`): Input mesh.
        fins_only (``bool``): If set, only remove fins.

    Returns:
        2 values are returned.

            * ``output_mesh`` (:class:`Mesh`): Output mesh.
            * ``information`` (:class:`dict`): A ``dict`` of additional informations.

        The following fields are defined in ``information``:

            * ``ori_face_index``: An array of original face indices. I.e. face
              ``i`` of the ``output_faces`` has index ``ori_face_index[i]`` in
              the input vertices.

    """
    vertices, faces, info = remove_duplicated_faces_raw(
            mesh.vertices, mesh.faces, fins_only);
    return form_mesh(vertices, faces), info;
