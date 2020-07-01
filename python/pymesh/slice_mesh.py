from .boolean import boolean
from .misc import Quaternion
from .meshutils import generate_box_mesh, separate_mesh, remove_isolated_vertices, merge_meshes
from .meshio import form_mesh, save_mesh

import numpy as np
from numpy.linalg import norm

def slice_mesh(mesh, direction, N):
    """ Slice a given 3D mesh N times along certain direciton.

    Args:
        mesh (:class:`Mesh`): The mesh to be sliced.
        direction (:class:`numpy.ndaray`): Direction orthogonal to the slices.
        N (int): Number of slices.

    Returns:
        A list of `N` :class:`Mesh` objects, each representing a single slice.
    """
    if mesh.dim != 3:
        raise NotImplementedError("Only slicing 3D mesh is supported.")

    bbox_min, bbox_max = mesh.bbox
    center = 0.5 * (bbox_min + bbox_max)
    radius = norm(bbox_max - center)
    direction = np.array(direction)
    direction = direction / norm(direction)

    proj_len = np.dot(mesh.vertices, direction)
    min_val = np.amin(proj_len)
    max_val = np.amax(proj_len)
    mid_val = 0.5 * (min_val + max_val)
    intercepts = np.linspace(min_val - mid_val, max_val - mid_val, N+2)[1:-1]
    assert(len(intercepts) == N)
    if N%2 == 1:
        intercepts = np.append(intercepts, intercepts[-1]+radius)

    boxes = []
    for low, high in intercepts.reshape((-1, 2), order="C"):
        min_corner = -np.ones(3) * (radius+1)
        max_corner =  np.ones(3) * (radius+1)
        min_corner[2] = low
        max_corner[2] = high
        box = generate_box_mesh(min_corner, max_corner)
        boxes.append(box)

    num_boxes = len(boxes)
    boxes = merge_meshes(boxes)
    rot = Quaternion.fromData(
            np.array([0.0, 0.0, 1.0]), np.array(direction)).to_matrix()
    boxes = form_mesh(np.dot(rot, boxes.vertices.T).T + center, boxes.faces)

    slabs = boolean(boxes, mesh, "intersection")

    cross_secs = []
    source = slabs.get_attribute("source").ravel()
    selected = source == 1
    cross_section_faces = slabs.faces[selected]
    cross_section = form_mesh(slabs.vertices, cross_section_faces)

    intersects = np.dot(slabs.vertices, direction).ravel() - \
            np.dot(center, direction)
    eps = (max_val - min_val) / (2 * N)

    for i,val in enumerate(intercepts[:N]):
        selected_vertices = np.logical_and(
                intersects > val - eps,
                intersects < val + eps)
        selected_faces = np.all(selected_vertices[cross_section_faces], axis=1).ravel()
        faces = cross_section_faces[selected_faces]
        if i%2 == 0:
            faces = faces[:,[0, 2, 1]]
        m = form_mesh(slabs.vertices, faces)
        m = remove_isolated_vertices(m)[0]
        cross_secs.append(m)

    return cross_secs

