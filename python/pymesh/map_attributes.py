import PyMesh
import numpy as np
import numpy.linalg
from .aabb_tree import BVH

def map_vertex_attribute(mesh1, mesh2, attr_name, bvh=None):
    """ Map vertex attribute from mesh1 to mesh2 based on closest points.

    Args:
        mesh1 (:class:`Mesh`): Source mesh, where the attribute is defined.
        mesh2 (:class:`Mesh`): Target mesh, where the attribute is mapped to.
        attr_name (``string``): Attribute name.
        bvh (:class:`BVH`): Pre-computed Bounded volume hierarchy if available.

    A new attribute with name ``attr_name`` is added to ``mesh2``.
    """

    assert(mesh1.dim == mesh2.dim)
    assert(mesh1.vertex_per_face == 3)
    assert(mesh2.vertex_per_face == 3)
    assert(mesh1.has_attribute(attr_name))
    values = mesh1.get_vertex_attribute(attr_name)

    if bvh is None:
        bvh = BVH(dim=mesh1.dim)
        bvh.load_mesh(mesh1)
    dists, closest_faces, p = bvh.lookup(mesh2.vertices)

    vertices = mesh1.vertices
    faces = mesh1.faces
    v0 = vertices[faces[closest_faces,0],:]
    v1 = vertices[faces[closest_faces,1],:]
    v2 = vertices[faces[closest_faces,2],:]

    a01 = np.linalg.norm(np.cross(v0-p, v1-p), axis=1).reshape((-1,1))
    a12 = np.linalg.norm(np.cross(v1-p, v2-p), axis=1).reshape((-1,1))
    a20 = np.linalg.norm(np.cross(v2-p, v0-p), axis=1).reshape((-1,1))

    a = a01 + a12 + a20

    val_0 = values[faces[closest_faces, 0],:]
    val_1 = values[faces[closest_faces, 1],:]
    val_2 = values[faces[closest_faces, 2],:]

    target_val = (val_0 * a12 + val_1 * a20 + val_2 * a01) / a

    if not mesh2.has_attribute(attr_name):
        mesh2.add_attribute(attr_name)
    mesh2.set_attribute(attr_name, target_val)


def map_face_attribute(mesh1, mesh2, attr_name, bvh=None):
    """ Map face attribute from mesh1 to mesh2 based on closest points.

    Args:
        mesh1 (:class:`Mesh`): Source mesh, where the attribute is defined.
        mesh2 (:class:`Mesh`): Target mesh, where the attribute is mapped to.
        attr_name (``string``): Attribute name.
        bvh (:class:`BVH`): Pre-computed Bounded volume hierarchy if available.

    A new attribute with name ``attr_name`` is added to ``mesh2``.
    """

    assert(mesh1.dim == mesh2.dim)
    assert(mesh1.has_attribute(attr_name))
    values = mesh1.get_face_attribute(attr_name)

    if not mesh2.has_attribute("face_centroid"):
        mesh2.add_attribute("face_centroid")
    query_pts = mesh2.get_face_attribute("face_centroid")

    if bvh is None:
        bvh = BVH(dim=mesh1.dim)
        bvh.load_mesh(mesh1)
    dists, closest_faces, p = bvh.lookup(query_pts)
    target_val = values[closest_faces, :]

    if not mesh2.has_attribute(attr_name):
        mesh2.add_attribute(attr_name)
    mesh2.set_attribute(attr_name, target_val)

def map_corner_attribute(mesh1, mesh2, attr_name, bvh=None):
    """ Map per-vertex per-face attribute from mesh1 to mesh2 based on closest points.

    Args:
        mesh1 (:class:`Mesh`): Source mesh, where the attribute is defined.
        mesh2 (:class:`Mesh`): Target mesh, where the attribute is mapped to.
        attr_name (``string``): Attribute name.
        bvh (:class:`BVH`): Pre-computed Bounded volume hierarchy if available.

    A new attribute with name ``attr_name`` is added to ``mesh2``.
    """

    assert(mesh1.dim == mesh2.dim)
    assert(mesh1.vertex_per_face == 3)
    assert(mesh2.vertex_per_face == 3)
    assert(mesh1.has_attribute(attr_name))
    values = mesh1.get_attribute(attr_name).reshape(
            (mesh1.num_faces, mesh1.vertex_per_face, -1))

    if not mesh2.has_attribute("face_centroid"):
        mesh2.add_attribute("face_centroid")
    centroids = mesh2.get_face_attribute("face_centroid")

    if bvh is None:
        bvh = BVH(dim=mesh1.dim)
        bvh.load_mesh(mesh1)

    v_dists, v_closest_faces, v_p = bvh.lookup(mesh2.vertices)
    f_dists, f_closest_faces, f_p = bvh.lookup(centroids)

    p0 = v_p[mesh2.faces[:,0], :]
    p1 = v_p[mesh2.faces[:,1], :]
    p2 = v_p[mesh2.faces[:,2], :]

    vertices = mesh1.vertices
    faces = mesh1.faces
    v0 = vertices[faces[f_closest_faces,0],:]
    v1 = vertices[faces[f_closest_faces,1],:]
    v2 = vertices[faces[f_closest_faces,2],:]

    p0_a01 = np.linalg.norm(np.cross(v0-p0, v1-p0), axis=1).reshape((-1,1))
    p0_a12 = np.linalg.norm(np.cross(v1-p0, v2-p0), axis=1).reshape((-1,1))
    p0_a20 = np.linalg.norm(np.cross(v2-p0, v0-p0), axis=1).reshape((-1,1))
    p1_a01 = np.linalg.norm(np.cross(v0-p1, v1-p1), axis=1).reshape((-1,1))
    p1_a12 = np.linalg.norm(np.cross(v1-p1, v2-p1), axis=1).reshape((-1,1))
    p1_a20 = np.linalg.norm(np.cross(v2-p1, v0-p1), axis=1).reshape((-1,1))
    p2_a01 = np.linalg.norm(np.cross(v0-p2, v1-p2), axis=1).reshape((-1,1))
    p2_a12 = np.linalg.norm(np.cross(v1-p2, v2-p2), axis=1).reshape((-1,1))
    p2_a20 = np.linalg.norm(np.cross(v2-p2, v0-p2), axis=1).reshape((-1,1))

    a = p0_a01 + p0_a12 + p0_a20

    val_p0 = (values[f_closest_faces, 0, :] * p0_a12 +\
            values[f_closest_faces, 1, :] * p0_a20 +\
            values[f_closest_faces, 2, :] * p0_a01) / a
    val_p1 = (values[f_closest_faces, 0, :] * p1_a12 +\
            values[f_closest_faces, 1, :] * p1_a20 +\
            values[f_closest_faces, 2, :] * p1_a01) / a
    val_p2 = (values[f_closest_faces, 0, :] * p2_a12 +\
            values[f_closest_faces, 1, :] * p2_a20 +\
            values[f_closest_faces, 2, :] * p2_a01) / a
    target_val = np.stack([val_p0, val_p1, val_p2], axis=1)

    if not mesh2.has_attribute(attr_name):
        mesh2.add_attribute(attr_name)
    mesh2.set_attribute(attr_name, target_val)
