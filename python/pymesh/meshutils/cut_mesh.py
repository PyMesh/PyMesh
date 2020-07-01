from PyMesh import MeshCutter
from ..Mesh import Mesh

import numpy as np

def cut_mesh(mesh, comp_ids=None):
    """ Cut mesh into connected components based on ``comp_ids``.  If
    ``comp_ids`` is ``None``, cut mesh based on UV discontinuity.

    Args:
        mesh (:class:`Mesh`): Input mesh.  If ``comp_ids`` is ``None``, it must
            have uv coordinates stored as per-corner attribute named
            ``corner_texture``.
        comp_ids (``numpy.ndarray``): Per-face label of how to group faces into
            connected components.

    Returns:
        The cutted output mesh object.
    """

    cutter = MeshCutter(mesh.raw_mesh)
    if comp_ids is not None:
        comp_ids = np.array(comp_ids)
        cutted_mesh = Mesh(cutter.cut_with_face_labels(comp_ids))
        cutted_mesh.add_attribute("comp_ids")
        cutted_mesh.set_attribute("comp_ids", comp_ids)
    else:
        cutted_mesh = Mesh(cutter.cut_at_uv_discontinuity())
        cutted_mesh.add_attribute("corner_texture")
        cutted_mesh.set_attribute("corner_texture",
                mesh.get_attribute("corner_texture"))
    return cutted_mesh

