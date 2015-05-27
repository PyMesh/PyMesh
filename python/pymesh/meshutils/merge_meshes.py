from ..meshio import form_mesh
import numpy as np

def merge_meshes(input_meshes):
    """ Merge multiple meshes into a single mesh.

    Args:
        input_meshes (``list``): a list of input :class:`Mesh` objects.

    Returns:

        ``Mesh``: An mesh consists of all vertices, faces and voxels
        from ``input_meshes``.
    """
    vertices = [];
    faces = [];
    voxels = [];
    vertex_count = 0;

    for mesh in input_meshes:
        vertices.append(mesh.vertices);
        if mesh.num_faces > 0:
            faces.append(mesh.faces + vertex_count);
        if mesh.num_voxels > 0:
            voxels.append(mesh.voxels + vertex_count);
        vertex_count += mesh.num_vertices;

    if len(vertices) > 0:
        vertices = np.vstack(vertices);
    else:
        vertices = np.zeros((0, 3), dtype=float);

    if len(faces) > 0:
        faces = np.vstack(faces);
    else:
        faces = np.zeros((0, 3), dtype=int)

    if len(voxels) > 0:
        voxels = np.vstack(voxels);
    else:
        voxels = np.zeros((0, 4), dtype=int);

    output_mesh = form_mesh(vertices, faces, voxels);
    return output_mesh;

