import os.path

import logging
import numpy as np

from PyMeshUtils import MeshSeparator

from ..meshio import form_mesh
from .remove_isolated_vertices import remove_isolated_vertices_raw

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
        A list of meshes, each represent a single connected component.  Each
        output component have the following attributes defined:

            * ``ori_vertex_index``: The input vertex index of each output vertex.
            * ``ori_elem_index``: The input element index of each output element.
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
        elem_sources = separator.get_sources(i).ravel();
        vertices, comp, info = remove_isolated_vertices_raw(
                mesh.vertices, comp);
        if is_voxel_mesh:
            comp_mesh = form_mesh(vertices, np.zeros((0, 3)), comp);
        else:
            comp_mesh = form_mesh(vertices, comp);
        comp_mesh.add_attribute("ori_vertex_index");
        comp_mesh.set_attribute("ori_vertex_index", info["ori_vertex_index"]);
        comp_mesh.add_attribute("ori_elem_index");
        comp_mesh.set_attribute("ori_elem_index", elem_sources);
        comp_meshes.append(comp_mesh);

    return comp_meshes;

def separate_graph(edges):
    """ Split graph into disconnected components.

    Args:
        edges (:class:`numpy.ndarray`): edges of the graph.

    Returns:
        An array of indices indicating the component each edge belongs to.
    """
    separator = MeshSeparator(edges);
    separator.set_connectivity_type(MeshSeparator.VERTEX);
    num_comps = separator.separate();

    comp_indices = np.zeros(len(edges));
    for i in range(num_comps):
        src_idx = separator.get_sources(i).ravel();
        comp_indices[src_idx] = i;
    return comp_indices;

