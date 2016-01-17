import PyIGL
import numpy as np
from .meshio import form_mesh
from .meshutils import remove_isolated_vertices

def partition_into_cells(mesh):
    partition = PyIGL.CellPartition.create_raw(mesh.vertices, mesh.faces);
    partition.run();

    vertices = partition.get_vertices();
    resolved_mesh = form_mesh(vertices, partition.get_faces());

    patch_ids = partition.get_patches().ravel();
    per_patch_cells = partition.get_cells();
    cell_ids = per_patch_cells[patch_ids];

    resolved_mesh.add_attribute("source_face");
    resolved_mesh.set_attribute("source_face", partition.get_source_faces());
    resolved_mesh.add_attribute("patches");
    resolved_mesh.set_attribute("patches", patch_ids);
    resolved_mesh.add_attribute("cells");
    resolved_mesh.set_attribute("cells", cell_ids[:,1].ravel());
    resolved_mesh.add_attribute("winding_number");
    resolved_mesh.set_attribute("winding_number",
            partition.get_winding_number()[:,1].ravel());

    num_cells = partition.get_num_cells();
    cell_faces = [partition.get_cell_faces(i) for i in range(num_cells)];
    cells = [form_mesh(vertices, f) for f in cell_faces];
    cells = [remove_isolated_vertices(m)[0] for m in cells];

    return resolved_mesh, cells;

