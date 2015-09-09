import numpy as np
import os
import os.path
import logging

from ..meshio import form_mesh, save_mesh, load_mesh
from .remove_isolated_vertices import remove_isolated_vertices_raw
from .remove_duplicated_vertices import remove_duplicated_vertices_raw

def generate_box_mesh(box_min, box_max,
        num_samples=1, keep_symmetry=False, subdiv_order=0, using_simplex=True):
    """ Generate axis-aligned box mesh.

    Each box is made of a number of cells (a square in 2D and cube in 3D), and
    each cell is made of triangles (2D) or tetrahedra (3D).

    Args:
        box_min (``numpy.ndarray``): min corner of the box.
        box_max (``numpy.ndarray``): max corner of the box.
        num_samples (``int``): (optional) Number of segments on each edge of the
            box.  Default is 1.
        keep_symmetry (``bool``): (optional) If true, ensure mesh connectivity
            respect all reflective symmetries of the box.  Default is true.
        subdiv_order (``int``): (optional) The subdivision order.  Default is 0.
        using_simplex (``bool``): If true, build box using simplex elements
            (i.e. triangle or tets), otherwise, use quad or hex element.

    Returns:
        :py:class:`Mesh`: The output box mesh.  The following attributes are
        defined:

            * ``cell_index``: An :py:class:`numpy.ndarray` of size :math:`N_e`
              that maps each element to the index of the cell it belongs to.
              :math:`N_e` is the number of elements.
    """
    if not isinstance(box_min, np.ndarray):
        box_min = np.array(box_min);
    if not isinstance(box_max, np.ndarray):
        box_max = np.array(box_max);
    dim = len(box_min);
    if dim == 2:
        mesh, cell_index = generate_2D_box_mesh(box_min, box_max, num_samples,
                keep_symmetry, subdiv_order, using_simplex);
    elif dim == 3:
        mesh, cell_index = generate_3D_box_mesh(box_min, box_max, num_samples,
                keep_symmetry, subdiv_order, using_simplex);

    mesh.add_attribute("cell_index");
    mesh.set_attribute("cell_index", cell_index);
    return mesh

def generate_2D_box_mesh(box_min, box_max, num_samples, keep_symmetry,
        subdiv_order, using_simplex):
    if isinstance(num_samples, int):
        num_samples = [num_samples, num_samples];
    step_size = np.divide((box_max - box_min), num_samples);

    num_vertices = 0;
    vertices = [];
    faces = [];
    quad_indices = [];
    quad_index = 0;
    for i in range(num_samples[0]):
        for j in range(num_samples[1]):
            p = np.multiply([i, j], step_size) + box_min;
            corners = np.array([
                    [p[0]             , p[1]             ],
                    [p[0]+step_size[0], p[1]             ],
                    [p[0]+step_size[0], p[1]+step_size[1]],
                    [p[0]             , p[1]+step_size[1]] ]);
            subcell_corners = subdivide_quad(corners, subdiv_order);
            for corners in subcell_corners:
                if using_simplex:
                    if keep_symmetry:
                        cell_vertices, cell_faces =\
                                split_quad_into_tris_symmetrically(corners);
                    else:
                        cell_vertices, cell_faces =\
                                split_quad_into_tris(corners);
                else:
                    cell_vertices = corners;
                    cell_faces = np.array([[0, 1, 2, 3]]);
                vertices.append(cell_vertices);
                faces.append(cell_faces + num_vertices);
                num_vertices += len(cell_vertices);
                quad_indices.append(np.ones(len(cell_faces)) * quad_index);

            quad_index +=1;

    vertices = np.vstack(vertices);
    faces = np.vstack(faces);
    quad_indices = np.vstack(quad_indices).ravel(order="C");

    vertices, faces, __ = remove_duplicated_vertices_raw(vertices, faces);
    vertices, faces, __ = remove_isolated_vertices_raw(vertices, faces);

    tets = np.array([], dtype=int);
    mesh = form_mesh(vertices, faces, tets);
    return mesh, quad_indices;

def subdivide_quad(corners, subdiv_order):
    """
    Vertex indices   Edge indices
                         2
    3 +------+ 2     +------+
      |      |     3 |      | 1
      |      |       |      |
    0 +------+ 1     +------+
                         0
    """
    if subdiv_order == 0:
        return [corners];
    face_center = np.mean(corners, axis=0);
    edge_centers = [
            0.5 * (corners[0] + corners[1]),
            0.5 * (corners[1] + corners[2]),
            0.5 * (corners[2] + corners[3]),
            0.5 * (corners[3] + corners[0]) ];

    subcells = [
            [corners[0], edge_centers[0], face_center, edge_centers[3]],
            [edge_centers[0], corners[1], edge_centers[1], face_center],
            [face_center, edge_centers[1], corners[2], edge_centers[2]],
            [edge_centers[3], face_center, edge_centers[2], corners[3]] ];

    subcell_corners = [];
    for cell in subcells:
        subcell_corners += subdivide_quad(cell, subdiv_order-1);
    return subcell_corners;

def split_quad_into_tris(corners):
    vertices = corners;
    faces = np.array([[0, 1, 2], [0, 2, 3]]);
    return vertices, faces;

def split_quad_into_tris_symmetrically(corners):
    center = np.mean(corners, axis=0);
    vertices = np.vstack((corners, center.reshape((1, -1))));
    faces = np.array([
            [0, 1, 4],
            [1, 2, 4],
            [2, 3, 4],
            [3, 0, 4] ]);
    return vertices, faces;

def reorientate_triangles(vertices, faces):
    """ This only works for convex shapes
    """
    centroid = np.mean(vertices, axis=0);
    face_centers = np.mean(vertices[faces], axis=1);
    out_dir = face_centers - centroid;
    edge_1= vertices[faces[:,1]] - vertices[faces[:,0]]
    edge_2= vertices[faces[:,2]] - vertices[faces[:,0]]
    normals = np.cross(edge_1, edge_2);

    pointing_inwards = np.sum(normals * out_dir, axis=1) < 0.0;
    faces[pointing_inwards, :] = faces[pointing_inwards][:,[0,2,1]];
    return faces;

def reorientate_tets(vertices, tets):
    edge_1 = vertices[tets[:,1]] - vertices[tets[:,0]];
    edge_2 = vertices[tets[:,2]] - vertices[tets[:,0]];
    edge_3 = vertices[tets[:,3]] - vertices[tets[:,0]];
    volume = np.sum(np.cross(edge_1, edge_2) * edge_3, axis=1);
    bad = np.absolute(volume) < 1e-6;
    logging.debug("Detected {} inverted tets".format(np.count_nonzero(bad)));
    inverted = volume < 0.0;
    tets[inverted] = tets[inverted][:, [0,1,3,2]];
    tets = tets[np.logical_not(bad)];
    return tets;

def generate_3D_box_mesh(bbox_min, bbox_max, num_samples, keep_symmetry=False,
        subdiv_order=0, using_simplex=True):
    if isinstance(num_samples, int):
        num_samples = [num_samples, num_samples, num_samples];
    step_size = np.divide((bbox_max - bbox_min), num_samples);

    num_vertices = 0;
    vertices = [];
    tets = [];
    hex_indices = [];
    hex_index = 0;
    for i in range(num_samples[0]):
        for j in range(num_samples[1]):
            for k in range(num_samples[2]):
                p = np.multiply([i,j,k], step_size) + bbox_min;
                corners = [
                        [p[0]             , p[1]             , p[2]             ],
                        [p[0]+step_size[0], p[1]             , p[2]             ],
                        [p[0]+step_size[0], p[1]+step_size[1], p[2]             ],
                        [p[0]             , p[1]+step_size[1], p[2]             ],
                        [p[0]             , p[1]             , p[2]+step_size[2]],
                        [p[0]+step_size[0], p[1]             , p[2]+step_size[2]],
                        [p[0]+step_size[0], p[1]+step_size[1], p[2]+step_size[2]],
                        [p[0]             , p[1]+step_size[1], p[2]+step_size[2]],
                        ];
                subcell_corners = subdivide_hex(corners, subdiv_order);
                for corners in subcell_corners:
                    if using_simplex:
                        if keep_symmetry:
                            cell_vertices, cell_elems =\
                                    split_hex_into_tets_symmetrically(corners);
                        else:
                            cell_vertices, cell_elems =\
                                    split_hex_into_tets(corners);
                    else:
                        cell_vertices = corners;
                        cell_elems = np.array([[0, 1, 2, 3, 4, 5, 6, 7]]);
                    vertices.append(cell_vertices);
                    tets.append(cell_elems + num_vertices);
                    num_vertices += len(cell_vertices);
                    hex_indices.append(np.ones(len(cell_elems)) * hex_index);

                hex_index +=1;

    vertices = np.vstack(vertices);
    tets = np.vstack(tets);
    hex_indices = np.vstack(hex_indices).ravel(order="C");

    vertices, tets, __ = remove_duplicated_vertices_raw(vertices, tets);
    vertices, tets, __ = remove_isolated_vertices_raw(vertices, tets);

    faces = np.array([], dtype=int);
    mesh = form_mesh(vertices, faces, tets);
    return mesh, hex_indices;

def subdivide_hex(corners, order):
    """ Subdivide hex into 8**order sub-cells.
    Corner ordering:     Edge orders:
         7 _______ 6            ___2___
          /:     /|           7/:    6/|        z
       4 /______/ |           /___1__/ |        |
        |  :   5| |          | 11    | |10      |  y
        |  :... |.|         8|  :.3. |.|        | /
        | . 3   | /2         | 4    9| /        |/
        |_______|/           |_______|/5        /-------x
        0        1               0
    """
    if order == 0:
        return [corners];
    else:
        corners = np.array(corners);
        centroid = np.mean(corners, axis=0);

        faces = np.array([
            [0, 3, 2, 1], # bottom
            [4, 5, 6, 7], # top
            [1, 2, 6, 5], # right
            [0, 4, 7, 3], # left
            [0, 1, 5, 4], # front
            [7, 6, 2, 3], # back
            ]);
        face_centers = np.array([np.mean(corners[face], axis=0)
            for face in faces]);

        edge = np.array([
            [0, 1], [4, 5], [7, 6], [3, 2], # along X
            [0, 3], [1, 2], [5, 6], [4, 7], # along Y
            [0, 4], [1, 5], [2, 6], [3, 7], # along Z
            ]);
        edge_centers = np.mean(corners[edge], axis=1);

        subcells = [
                [corners[0], edge_centers[0], face_centers[0], edge_centers[4],
                 edge_centers[8], face_centers[4], centroid, face_centers[3] ],
                [edge_centers[0], corners[1], edge_centers[5], face_centers[0],
                 face_centers[4], edge_centers[9], face_centers[2], centroid],
                [edge_centers[4], face_centers[0], edge_centers[3], corners[3],
                 face_centers[3], centroid, face_centers[5], edge_centers[11]],
                [face_centers[0], edge_centers[5], corners[2], edge_centers[3],
                 centroid, face_centers[2], edge_centers[10], face_centers[5]],

                [edge_centers[8], face_centers[4], centroid, face_centers[3],
                 corners[4], edge_centers[1], face_centers[1], edge_centers[7]],
                [face_centers[4], edge_centers[9], face_centers[2], centroid,
                 edge_centers[1], corners[5], edge_centers[6], face_centers[1]],
                [face_centers[3], centroid, face_centers[5], edge_centers[11],
                 edge_centers[7], face_centers[1], edge_centers[2], corners[7]],
                [centroid, face_centers[2], edge_centers[10], face_centers[5],
                 face_centers[1], edge_centers[6], corners[6], edge_centers[2]] ];
        subcell_corners = [];
        for cell in subcells:
            subcell_corners += subdivide_hex(cell, order-1);
        return subcell_corners;

def split_hex_into_tets(corners):
    """ Convert a hex to 24 tets.
    Algorithm: Each hex is split into 6 tets.  The resulting tet mesh does not
    symmetric connectivities.

    Corner ordering:
         7 _______ 6
          /:     /|        z
       4 /______/ |        |
        |  :   5| |        |  y
        |  :... |.|        | /
        | . 3   | /2       |/
        |_______|/         /-------x
        0        1

    """
    vertices = np.array(corners);
    tets = np.array([
        [0, 3, 7, 6],
        [0, 3, 6, 2],
        [0, 2, 6, 1],
        [5, 0, 6, 1],
        [5, 0, 4, 6],
        [6, 0, 4, 7],
        ]);
    return vertices, tets;

def split_hex_into_tets_symmetrically(corners):
    """ Convert a hex to 24 tets.
    Algorithm: Form 6 pyramids by using cell centroid and each face.  Break each
    pyramid into 4 tets by spliting the base (the original cell face) into 4 triangles.

    Corner ordering:
         7 _______ 6
          /:     /|        z
       4 /______/ |        |
        |  :   5| |        |  y
        |  :... |.|        | /
        | . 3   | /2       |/
        |_______|/         /-------x
        0        1

    """
    assert(len(corners) == 8);
    corners = np.array(corners);
    centroid = np.mean(corners, axis=0);
    faces = np.array([
        [0, 3, 2, 1], # bottom
        [4, 5, 6, 7], # top
        [1, 2, 6, 5], # right
        [0, 4, 7, 3], # left
        [0, 1, 5, 4], # front
        [7, 6, 2, 3], # back
        ]);

    face_centers = np.array([np.mean(corners[face], axis=0)
        for face in faces]);

    vertices = np.vstack([corners, face_centers, centroid]);
    tets = np.array([
        # tets based on bottom face
        [0, 1, 8, 14],
        [1, 2, 8, 14],
        [2, 3, 8, 14],
        [3, 0, 8, 14],
        # tets based on top faces
        [5, 4, 9, 14],
        [4, 7, 9, 14],
        [7, 6, 9, 14],
        [6, 5, 9, 14],
        # tets based on right face
        [2, 1, 10, 14],
        [1, 5, 10, 14],
        [5, 6, 10, 14],
        [6, 2, 10, 14],
        # tets based on left face
        [0, 3, 11, 14],
        [3, 7, 11, 14],
        [7, 4, 11, 14],
        [4, 0, 11, 14],
        # tets based on front face
        [0, 4, 12, 14],
        [4, 5, 12, 14],
        [5, 1, 12, 14],
        [1, 0, 12, 14],
        # tets based on back face
        [3, 2, 13, 14],
        [2, 6, 13, 14],
        [6, 7, 13, 14],
        [7, 3, 13, 14],
        ]);

    return vertices, tets;


