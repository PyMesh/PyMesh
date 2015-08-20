import os.path

import logging
import numpy as np

from PyMeshUtils import MeshSeparator

from ..meshio import form_mesh
from remove_isolated_vertices import remove_isolated_vertices_raw

def separate_mesh(mesh, connectivity_type="auto"):
    """ Split mesh into connected components.

    Args:
        mesh (:class:`Mesh`): Input mesh.
        connectivity_type (:py:class:`str`): possible types are

            * ``auto``: Same as ``face`` for surface mesh, ``voxel`` for voxel mesh.
            * ``vertex``: Group component based on vertex connectivity.
            * ``face``: Group component based on face connectivity.
            * ``voxel``: Group component based on voxel connectivity.

    Returns:
        A list of meshes, each represent a single connected component.
    """
    is_voxel_mesh = mesh.num_voxels > 0;
    if connectivity_type == "auto":
        if is_voxel_mesh:
            connectivity_type = "voxel";
        else:
            connectivity_type = "face";

    if connectivity_type == "vertex":
        if is_voxel_mesh > 0:
            separator = MeshSeparator(mesh.voxels);
        else:
            separator = MeshSeparator(mesh.faces);
        separator.set_connectivity_type(MeshSeparator.VERTEX);
    elif connectivity_type == "face":
        separator = MeshSeparator(mesh.faces);
        separator.set_connectivity_type(MeshSeparator.FACE);
    elif connectivity_type == "voxel":
        separator = MeshSeparator(mesh.voxels);
        separator.set_connectivity_type(MeshSeparator.VOXEL);
    else:
        raise RuntimeError("Unsupported connectivity type: {}".format(
            connectivity_type));

    num_comps = separator.separate();

    comp_meshes = [];
    for i in range(num_comps):
        comp = separator.get_component(i);
        vertices, comp, __ = remove_isolated_vertices_raw(
                mesh.vertices, comp);
        if is_voxel_mesh:
            comp_meshes.append(form_mesh(vertices, np.zeros((0, 3)), comp));
        else:
            comp_meshes.append(form_mesh(vertices, comp));

    return comp_meshes;

