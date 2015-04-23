from PyMeshUtils import FinFaceRemoval

def remove_duplicated_faces(vertices, faces):
    """ Remove duplicated faces.

    Duplicated faces are defined as faces consist of the same set of vertices.
    Depending on the face orientation, there could be two different outcomes.

    If there is a dominant orientation, that is more than half of the faces are
    consistently orientated, one face with the dominant orientation will be
    kept while all other faces are removed.

    If there is no dominant orientation, i.e. half of the face are positively
    orientated and the other half is negatively orientated, all faces are
    discarded.

    The reason for this design is that when there is no dominant orientation,
    the faces are likely to be "fins."  Thus they are removed completely.  On
    the other hand, when there exists a dominant orientation, the faces are
    likely from "folds", so one face should be kept.

    Args:
        vertices (``numpy.ndarray``): Vertex array with one vertex per row.
        faces (``numpy.ndarray``): Face array with one face per row.

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
    remover.run();
    info = {
            "ori_face_index": remover.get_face_indices(),
            };
    return remover.get_vertices(), remover.get_faces(), info;
