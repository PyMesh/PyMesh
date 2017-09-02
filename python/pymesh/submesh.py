import numpy as np
from .meshutils import remove_isolated_vertices_raw
from .meshio import form_mesh

def expand_by_one_ring(vertices, elements, selected_elements):
    num_vertices = vertices.shape[0];
    selected_vertices = np.zeros(num_vertices, dtype=bool);
    selected_vertices[elements[selected_elements].ravel()] = True;
    selected_elements = np.any(selected_vertices[elements], axis=1);
    return selected_elements;

def extract_submesh_surface(mesh, face_indices, n_ring):
    vertices = mesh.vertices;

    selected_faces = np.zeros(mesh.num_faces, dtype=bool);
    selected_faces[face_indices] = True;
    ring_index = np.zeros(mesh.num_faces, dtype=int);
    ring_index[selected_faces] += 1;
    for i in range(n_ring):
        selected_faces = expand_by_one_ring(
                mesh.vertices, mesh.faces, selected_faces);
        ring_index[selected_faces] += 1;
    selected_face_indices = np.arange(mesh.num_faces)[selected_faces];
    faces = mesh.faces[selected_faces];
    ring_index = (ring_index[selected_faces] - n_ring - 1) * (-1);

    vertices, faces, __ = remove_isolated_vertices_raw(vertices, faces);
    mesh = form_mesh(vertices, faces);
    mesh.add_attribute("ori_face_index");
    mesh.set_attribute("ori_face_index", selected_face_indices);
    mesh.add_attribute("ring");
    mesh.set_attribute("ring", ring_index);
    return mesh;

def extract_submesh_volume(mesh, tet_indices, n_ring):
    vertices = mesh.vertices;

    selected_voxels = np.zeros(mesh.num_voxels, dtype=bool);
    selected_voxels[tet_indices] = True;
    ring_index = np.zeros(mesh.num_voxels, dtype=int);
    ring_index[selected_voxels] += 1;
    for i in range(n_ring):
        selected_voxels = expand_by_one_ring(
                mesh.vertices, mesh.voxels, selected_voxels);
        ring_index[selected_voxels] += 1;
    selected_voxel_indices = np.arange(mesh.num_voxels)[selected_voxels];
    voxels = mesh.voxels[selected_voxels];
    ring_index = (ring_index[selected_voxels] - n_ring - 1) * (-1);

    vertices, voxels, __ = remove_isolated_vertices_raw(vertices, voxels);
    mesh = form_mesh(vertices, np.array([]), voxels);
    mesh.add_attribute("ori_voxel_index");
    mesh.set_attribute("ori_voxel_index", selected_voxel_indices);
    mesh.add_attribute("ring");
    mesh.set_attribute("ring", ring_index);
    return mesh;

def submesh(mesh, element_indices, num_rings):
    """ Extract a subset of the mesh elements and forming a new mesh.

    Args:
        mesh (:class:`Mesh`): The input mesh.
        element_indices (:class:`numpy.ndarray`): The indices of selected
            elements (faces/voxels).
        num_rings (`int`): The number of rings around the selected elements to
            extract.

    Returns:
       A mesh only containing the selected elements and their local neighborhood
       up to `num_rings` rings.  The output mesh contains the following
       attributes:

       * ori_face_index/ori_voxel_index: The original index of each element.
       * ring: Index indicating which ring does each element belongs.  The
           selected elements belongs to the 0-ring.
    """

    if mesh.num_voxels == 0:
        return extract_submesh_surface(mesh, element_indices, num_rings);
    else:
        return extract_submesh_volume(mesh, element_indices, num_rings);

