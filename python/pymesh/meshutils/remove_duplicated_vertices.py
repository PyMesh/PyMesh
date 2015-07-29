import numpy as np
from PyMeshUtils import DuplicatedVertexRemoval
from ..meshio import form_mesh

def remove_duplicated_vertices_raw(vertices, elements, tol=1e-12, importance=None):
    """ Merge duplicated vertices into a single vertex.

    Args:
        vertices (``numpy.ndarray``): Vertices in row major.
        elements (``numpy.ndarray``): Elements in row major.
        tol (``float``): (optional) Vertices with distance less than ``tol`` are
            considered as duplicates.  Default is ``1e-12``.
        importance (``numpy.ndarray``): (optional) Per-vertex importance value.
            When discarding duplicates, the vertex with the highest importance
            value will be kept.

    Returns:
        3 values are returned.

            * ``output_vertices``: Output vertices in row major.
            * ``output_elements``: Output elements in row major.
            * ``information``: A ``dict`` of additional informations.

        The following fields are defined in ``information``:

            * ``num_vertex_merged``: number of vertex merged.
            * ``index_map``: An array that maps input vertex index to output vertex index.
              I.e. vertex ``i`` will be mapped to ``index_map[i]`` in the output.

    """

    remover = DuplicatedVertexRemoval(vertices, elements);
    if importance is not None:
        if (len(importance) != len(vertices)):
            raise RuntimeError(
                    "Vertex importance must be of the same size as vertices");
        remover.set_importance_level(importance);
    num_merged = remover.run(tol);
    new_vertices = remover.get_vertices();
    new_elements = remover.get_faces();
    info = {
            "num_vertex_merged": num_merged,
            "index_map": remover.get_index_map().ravel(),
            };
    return new_vertices, new_elements, info;

def remove_duplicated_vertices(mesh, tol=1e-12, importance=None):
    """ Wrapper function of :func:`remove_duplicated_vertices_raw`.

    Args:
        mesh (:class:`Mesh`): Input mesh.
        tol (``float``): (optional) Vertices with distance less than ``tol`` are
            considered as duplicates.  Default is ``1e-12``.
        importance (``numpy.ndarray``): (optional) Per-vertex importance value.
            When discarding duplicates, the vertex with the highest importance
            value will be kept.

    Returns:
        2 values are returned.

            * ``output_mesh`` (:class:`Mesh`): Output mesh.
            * ``information`` (:class:`dict`): A ``dict`` of additional informations.

        The following fields are defined in ``information``:

            * ``num_vertex_merged``: number of vertex merged.
            * ``index_map``: An array that maps input vertex index to output vertex index.
              I.e. vertex ``i`` will be mapped to ``index_map[i]`` in the output.

    """
    if mesh.num_voxels == 0:
        vertices, faces, info = remove_duplicated_vertices_raw(
                mesh.vertices, mesh.faces, tol, importance);
        out_mesh = form_mesh(vertices, faces);
    else:
        vertices, voxels, info = remove_duplicated_vertices_raw(
                mesh.vertices, mesh.voxels, tol, importance);
        output_mesh = form_mesh(vertices, np.zeros((0, 3)), voxels);

    return out_mesh, info;
