import numpy as np
from PyMeshUtils import IsolatedVertexRemoval
from ..meshio import form_mesh

def remove_isolated_vertices_raw(vertices, elements):
    """ Remove isolated vertices.

    Args:
        vertices (``numpy.ndarray``): Vertex array with one vertex per row.
        elements (``numpy.ndarray``): Element array with one face per row.

    Returns:
        3 values are returned.

            * ``output_vertices``: Output vertex array with one vertex per row.
            * ``output_elements``: Output element array with one element per row.
            * ``infomation``: A ``dict`` of additional informations.

        The following fields are defined in ``infomation``:

            * ``num_vertex_removed``: Number of vertex removed.
            * ``ori_vertex_index``: Original vertex index.  That is vertex ``i`` of
              ``output_vertices`` has index ``ori_vertex_index[i]`` in the input
              vertex array.
    """
    remover = IsolatedVertexRemoval(vertices, elements);
    num_removed = remover.run();
    vertices = remover.get_vertices();
    elements = remover.get_faces();
    info = {
            "num_vertex_removed": num_removed,
            "ori_vertex_index": remover.get_ori_vertex_indices().ravel(),
            };
    return vertices, elements, info;

def remove_isolated_vertices(mesh):
    """ Wrapper function of :func:`remove_isolated_vertices_raw`.

    Args:
        mesh (:class:`Mesh`): Input mesh.

    Returns:
        2 values are returned.

            * ``output_mesh`` (:class:`Mesh`): Output mesh.
            * ``infomation`` (:class:`dict`): A ``dict`` of additional informations.

        The following fields are defined in ``infomation``:

            * ``num_vertex_removed``: Number of vertex removed.
            * ``ori_vertex_index``: Original vertex index.  That is vertex ``i`` of
              ``output_vertices`` has index ``ori_vertex_index[i]`` in the input
              vertex array.
    """
    if mesh.num_voxels == 0:
        vertices, faces, info = remove_isolated_vertices_raw(
                mesh.vertices, mesh.faces);
        output_mesh = form_mesh(vertices, faces);
    else:
        vertices, voxels, info = remove_isolated_vertices_raw(
                mesh.vertices, mesh.voxels);
        output_mesh = form_mesh(vertices, np.zeros((0, 3)), voxels);
    return output_mesh, info;

