import PyIGL
import numpy as np
from .meshio import form_mesh
from .meshutils import remove_isolated_vertices

def partition_into_cells(mesh):
    """ Resolve all-intersections of the input mesh and extract cell partitions
    induced by the mesh.  A cell-partition is subset of the ambient space where
    any pair of points belonging the partition can be connected by a curve
    without ever going through any mesh faces.

    Args:
        mesh (:class:`Mesh`): The input mesh.

    Returns: The output mesh with all intersections resolved and a list of
    meshes representing individual cells.

    The following attributes are defined in the output mesh:

        * ``source_face``: the original face index.
        * ``patches``: the scalar field marking manifold patches (A set of
          connected faces connected by manifold edges).
        * ``cells``: a per-face scalar field indicating the cell id on the
          positive side of each face.
        * ``winding_number``: the scalar field indicating the piece-wise
          constant winding number of the cell on the positive side of each face.
    """
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
    resolved_mesh.set_attribute("cells", cell_ids[:,0].ravel());
    resolved_mesh.add_attribute("winding_number");
    resolved_mesh.set_attribute("winding_number",
            partition.get_winding_number()[:,0].ravel());

    num_cells = partition.get_num_cells();
    cell_faces = [partition.get_cell_faces(i) for i in range(num_cells)];
    cells = [form_mesh(vertices, f) for f in cell_faces];
    cells = [remove_isolated_vertices(m)[0] for m in cells];

    return resolved_mesh, cells;

