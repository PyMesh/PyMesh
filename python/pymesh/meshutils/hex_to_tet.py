import numpy as np

from ..meshio import form_mesh
from .remove_isolated_vertices import remove_isolated_vertices_raw
from .remove_duplicated_vertices import remove_duplicated_vertices_raw
from .generate_box_mesh import split_hex_into_tets, split_hex_into_tets_symmetrically
from .generate_box_mesh import subdivide_hex

def hex_to_tet(mesh, keep_symmetry=False, subdiv_order=0):
    """
    Convert hex mesh into tet mesh.

    Args:
        mesh (:class:`Mesh`): Input hex mesh.
        keep_symmetry (`boolean`): (optional) Whether to split hex symmetrically
            into tets.  Default is False.
        subdiv_order (`int`): (optional) Number of times to subdiv the hex
            before splitting.  Default is 0.

    Returns:
        The resulting tet mesh.
    """
    assert(mesh.num_voxels > 0);
    num_vertices = 0;
    vertices = [];
    tets = [];
    hex_indices = [];
    for hex_index, corners in enumerate(mesh.voxels):
        corners = mesh.vertices[corners];
        subcell_corners = subdivide_hex(corners, subdiv_order);
        for corners in subcell_corners:
            if keep_symmetry:
                cell_vertices, cell_elems =\
                        split_hex_into_tets_symmetrically(corners);
            else:
                cell_vertices, cell_elems =\
                        split_hex_into_tets(corners);
            vertices.append(cell_vertices);
            tets.append(cell_elems + num_vertices);
            num_vertices += len(cell_vertices);
            hex_indices.append(np.ones(len(cell_elems)) * hex_index);

    vertices = np.vstack(vertices);
    tets = np.vstack(tets);
    hex_indices = np.vstack(hex_indices).ravel(order="C");

    vertices, tets, __ = remove_duplicated_vertices_raw(vertices, tets);
    vertices, tets, __ = remove_isolated_vertices_raw(vertices, tets);

    faces = np.array([], dtype=int);
    mesh = form_mesh(vertices, faces, tets);
    mesh.add_attribute("cell_index");
    mesh.set_attribute("cell_index", hex_indices);
    return mesh;
