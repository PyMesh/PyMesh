import numpy as np
from ..meshio import form_mesh

def mesh_to_graph(mesh):
    """
    Convert a mesh into a graph :math:`(V, E)`, where :math:`V` represents the
    mesh vertices, and :math:`E` represent the edges.

    Args:
        mesh (:class:`Mesh`): Input mesh.

    Returns:
        The graph :math:`(V, E)`.
    """

    if mesh.num_voxels > 0:
        # Avoid extracting voxel edges.
        vertices = mesh.vertices
        faces = mesh.faces
        mesh = form_mesh(vertices, faces)

    mesh.enable_connectivity()
    vertices = mesh.vertices
    edges = []
    for vi in range(mesh.num_vertices):
        adj_vertices = mesh.get_vertex_adjacent_vertices(vi)
        for vj in adj_vertices:
            if vj > vi:
                edges.append((vi, vj))
    return vertices, np.array(edges, dtype=int)

def mesh_to_dual_graph(mesh):
    """
    Convert a mesh into a dual graph :math:`(V, E)`, where :math:`V` represents
    the mesh faces, and :math:`E` represent the dual edges.

    Args:
        mesh (:class:`Mesh`): Input mesh.

    Returns:
        The graph :math:`(V, E)`.
    """

    mesh.enable_connectivity()
    mesh.add_attribute("face_centroid")
    vertices = mesh.get_face_attribute("face_centroid")
    edges = []
    for fi in range(mesh.num_faces):
        adj_faces = mesh.get_face_adjacent_faces(fi)
        for fj in adj_faces:
            if fj > fi:
                edges.append((fi, fj))
    return vertices, np.array(edges, dtype=int)
