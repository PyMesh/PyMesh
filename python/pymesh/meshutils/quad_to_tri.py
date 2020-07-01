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
    assert(mesh.vertex_per_face == 4)

    quads = mesh.faces
    if not keep_symmetry:
        triangles = np.array([
            [[f[0], f[1], f[2]],[f[0], f[2], f[3]]]
            for f in quads ]).reshape((-1, 3))
        quad_index = np.repeat(np.arange(len(quads), dtype=int), 2)
        tri_mesh = form_mesh(mesh.vertices, triangles)
        if mesh.has_attribute("corner_texture"):
            uv = mesh.get_attribute("corner_texture").reshape((-1, 2), order="C")
            uv = np.array([[
                uv[i*4,:], uv[i*4+1,:], uv[i*4+2],
                uv[i*4,:], uv[i*4+2,:], uv[i*4+3]] for i in range(len(quads))])
            tri_mesh.add_attribute("corner_texture")
            tri_mesh.set_attribute("corner_texture", uv.ravel(order="C"))
    else:
        num_vertices = mesh.num_vertices
        mesh.add_attribute("face_centroid")
        centers = mesh.get_face_attribute("face_centroid")
        triangles = np.array([
                [
                    [f[0], f[1], num_vertices+i],
                    [f[1], f[2], num_vertices+i],
                    [f[2], f[3], num_vertices+i],
                    [f[3], f[0], num_vertices+i],
                ] for i,f in enumerate(quads) ]).reshape((-1, 3))
        vertices = np.vstack([mesh.vertices, centers])
        quad_index = np.repeat(np.arange(len(quads), dtype=int), 4)
        tri_mesh = form_mesh(vertices, triangles)
        if mesh.has_attribute("corner_texture"):
            uv = mesh.get_attribute("corner_texture").reshape((-1, 4, 2), order="C")
            center_uv = np.mean(uv, axis=1)
            uv = uv.reshape((-1, 2))
            assert(len(center_uv == len(quads)))
            uv = np.array([[
                uv[i*4  ,:], uv[i*4+1,:], center_uv[i,:],
                uv[i*4+1,:], uv[i*4+2,:], center_uv[i,:],
                uv[i*4+2,:], uv[i*4+3,:], center_uv[i,:],
                uv[i*4+3,:], uv[i*4  ,:], center_uv[i,:]]
                for i in range(len(quads)) ])
            tri_mesh.add_attribute("corner_texture")
            tri_mesh.set_attribute("corner_texture", uv.ravel(order="C"))

    tri_mesh.add_attribute("cell_index")
    tri_mesh.set_attribute("cell_index", quad_index)
    return tri_mesh
