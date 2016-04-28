from .boolean import boolean
from .misc import Quaternion
from .meshutils import generate_box_mesh, separate_mesh, remove_isolated_vertices, merge_meshes
from .meshio import form_mesh, save_mesh

import numpy as np
from numpy.linalg import norm

def slice_mesh(mesh, direction, N):
    bbox_min, bbox_max = mesh.bbox;
    center = 0.5 * (bbox_min + bbox_max);
    radius = norm(bbox_max - center);

    proj_len = np.dot(mesh.vertices, direction);
    min_val = np.amin(proj_len);
    max_val = np.amax(proj_len);
    intercepts = np.linspace(min_val, max_val, N+2)[1:-1];
    assert(len(intercepts) == N);
    if N%2 == 1:
        intercepts = np.append(intercepts, intercepts[-1]+radius);

    boxes = [];
    for low, high in intercepts.reshape((-1, 2), order="C"):
        min_corner = center + np.array([-radius-1, -radius-1, low]);
        max_corner = center + np.array([ radius+1,  radius+1, high]);
        box = generate_box_mesh(min_corner, max_corner);
        boxes.append(box);

    num_boxes = len(boxes);
    boxes = merge_meshes(boxes);
    rot = Quaternion.fromData(
            np.array([0.0, 0.0, 1.0]), np.array(direction)).to_matrix();
    boxes = form_mesh(np.dot(rot, boxes.vertices.T).T, boxes.faces);

    slabs = boolean(boxes, mesh, "intersection");

    cross_secs = [];
    source_faces = slabs.get_attribute("source_face").ravel();
    for i in range(num_boxes):
        selected = np.logical_and(source_faces >= 12*i,
                source_faces < 12*i+12);
        cross_section_faces = slabs.faces[selected];
        cross_section = form_mesh(slabs.vertices, cross_section_faces);
        cross_secs = cross_secs + separate_mesh(cross_section);

    cross_secs = [remove_isolated_vertices(m)[0] for m in cross_secs];
    intersects = [np.dot(m.vertices[0], direction) for m in cross_secs];
    order = np.argsort(intersects);
    cross_secs = [cross_secs[i] for i in order];

    for i in range(0, len(cross_secs), 2):
        # Correct normal direction of every other slice.
        m = cross_secs[i];
        cross_secs[i] = form_mesh(m.vertices, m.faces[:, [0, 2, 1]]);
    return cross_secs;

