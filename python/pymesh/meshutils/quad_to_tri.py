import numpy as np
from ..meshio import form_mesh

def quad_to_tri(mesh, keep_symmetry=False):
    """
    Convert quad mesh into triangles.

    Args:
        mesh (:class:`Mesh`): Input quad mesh.
        keep_symmetry (`boolean`): (optional) Whether to split quad
            symmetrically into triangles.  Default is False.

    Returns:
        The resulting triangle mesh.
    """
    assert(mesh.vertex_per_face == 4);

    quads = mesh.faces;
    if not keep_symmetry:
        triangles = np.array([
            [[f[0], f[1], f[2]],[f[0], f[2], f[3]]]
            for f in quads ]).reshape((-1, 3));
        quad_index = np.repeat(np.arange(len(quads), dtype=int), 2);
        mesh = form_mesh(mesh.vertices, triangles);
    else:
        num_vertices = mesh.num_vertices;
        mesh.add_attribute("face_centroid");
        centers = mesh.get_face_attribute("face_centroid");
        triangles = np.array([
                [
                    [f[0], f[1], num_vertices+i],
                    [f[1], f[2], num_vertices+i],
                    [f[2], f[3], num_vertices+i],
                    [f[3], f[0], num_vertices+i],
                ] for i,f in enumerate(quads) ]).reshape((-1, 3));
        vertices = np.vstack([mesh.vertices, centers]);
        quad_index = np.repeat(np.arange(len(quads), dtype=int), 4);
        mesh = form_mesh(vertices, triangles);

    mesh.add_attribute("cell_index");
    mesh.set_attribute("cell_index", quad_index);
    return mesh;
