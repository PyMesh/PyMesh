import PyMesh
from .meshio import form_mesh

def cut_to_disk(mesh):
    """ Cut a mesh open into disk topology.  (Note the cut may not be optimal.)
    This method assumes the input mesh is edge-manifold.

    Args:
        mesh (:class:`Mesh`): Input mesh.

    Returns: The same mesh as input but cutted so it is topologically equivalent
    to a disk.
    """
    cutter = PyMesh.DiskCutter.create(mesh.raw_mesh)
    cutter.run()
    return form_mesh(cutter.vertices, cutter.faces)
