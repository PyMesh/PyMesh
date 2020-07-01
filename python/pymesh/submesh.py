import numpy as np
from .meshutils import remove_isolated_vertices_raw
from .meshio import form_mesh

def expand_by_one_ring(vertices, elements, selected_elements):
    num_vertices = vertices.shape[0]
    selected_vertices = np.zeros(num_vertices, dtype=bool)
    selected_vertices[elements[selected_elements].ravel()] = True
    selected_elements = np.any(selected_vertices[elements], axis=1)
    return selected_elements

def extract_submesh_surface(mesh, face_indices, n_ring):
    vertices = mesh.vertices

    selected_faces = np.zeros(mesh.num_faces, dtype=bool)
    selected_faces[face_indices] = True
    ring_index = np.zeros(mesh.num_faces, dtype=int)
    ring_index[selected_faces] += 1
    for i in range(n_ring):
        selected_faces = expand_by_one_ring(
                mesh.vertices, mesh.faces, selected_faces)
        ring_index[selected_faces] += 1
    selected_face_indices = np.arange(mesh.num_faces)[selected_faces]
    faces = mesh.faces[selected_faces]
    ring_index = (ring_index[selected_faces] - n_ring - 1) * (-1)

    vertices, faces, info = remove_isolated_vertices_raw(vertices, faces)
    out_mesh = form_mesh(vertices, faces)
    out_mesh.add_attribute("ori_face_index")
    out_mesh.set_attribute("ori_face_index", selected_face_indices)
    out_mesh.add_attribute("ring")
    out_mesh.set_attribute("ring", ring_index)

    # Transport attributes to output mesh.
    for name in mesh.attribute_names:
        attr = mesh.get_attribute(name)
        if len(attr) % mesh.num_vertices == 0:
            attr = attr.reshape((mesh.num_vertices, -1), order="C")
            attr = attr[info["ori_vertex_index"]]
            out_mesh.add_attribute(name)
            out_mesh.set_attribute(name, attr)
        elif len(attr) % mesh.num_faces == 0:
            attr = attr.reshape((mesh.num_faces, -1), order="C")
            attr = attr[selected_faces]
            out_mesh.add_attribute(name)
            out_mesh.set_attribute(name, attr)

    return out_mesh

def extract_submesh_volume(mesh, tet_indices, n_ring):
    vertices = mesh.vertices

    selected_voxels = np.zeros(mesh.num_voxels, dtype=bool)
    selected_voxels[tet_indices] = True
    ring_index = np.zeros(mesh.num_voxels, dtype=int)
    ring_index[selected_voxels] += 1
    for i in range(n_ring):
        selected_voxels = expand_by_one_ring(
                mesh.vertices, mesh.voxels, selected_voxels)
        ring_index[selected_voxels] += 1
    selected_voxel_indices = np.arange(mesh.num_voxels)[selected_voxels]
    voxels = mesh.voxels[selected_voxels]
    ring_index = (ring_index[selected_voxels] - n_ring - 1) * (-1)

    vertices, voxels, info = remove_isolated_vertices_raw(vertices, voxels)
    out_mesh = form_mesh(vertices, np.array([]), voxels)
    out_mesh.add_attribute("ori_voxel_index")
    out_mesh.set_attribute("ori_voxel_index", selected_voxel_indices)
    out_mesh.add_attribute("ring")
    out_mesh.set_attribute("ring", ring_index)

    # Transport attributes to output mesh.
    for name in mesh.attribute_names:
        attr = mesh.get_attribute(name)
        if len(attr) % mesh.num_vertices == 0:
            attr = attr.reshape((mesh.num_vertices, -1), order="C")
            attr = attr[info["ori_vertex_index"]]
            out_mesh.add_attribute(name)
            out_mesh.set_attribute(name, attr)
        elif len(attr) % mesh.num_voxels == 0:
            attr = attr.reshape((mesh.num_voxels, -1), order="C")
            attr = attr[selected_voxels]
            out_mesh.add_attribute(name)
            out_mesh.set_attribute(name, attr)

    return out_mesh

def submesh(mesh, element_indices, num_rings):
    """ Extract a subset of the mesh elements and forming a new mesh.

    Args:
        mesh (:class:`Mesh`): The input mesh.
        element_indices (:class:`numpy.ndarray`): The indices of selected
            elements (faces/voxels).
        num_rings (`int`): The number of rings around the selected elements to
            extract.

    Returns:
       A :class:`Mesh` object only containing the selected elements and their
       local neighborhood up to `num_rings` rings.  The output mesh contains the
       following attributes:

       * ori_face_index/ori_voxel_index: The original index of each element.
       * ring: Index indicating which ring does each element belongs.  The
         selected elements belongs to the 0-ring.
    """

    if mesh.num_voxels == 0:
        return extract_submesh_surface(mesh, element_indices, num_rings)
    else:
        return extract_submesh_volume(mesh, element_indices, num_rings)

