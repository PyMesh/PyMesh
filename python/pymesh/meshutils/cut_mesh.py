from PyMesh import MeshCutter
from ..Mesh import Mesh

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

    cutter = MeshCutter(mesh.raw_mesh);
    if comp_ids is not None:
        return Mesh(cutter.cut_with_face_labels(comp_ids));
    else:
        return Mesh(cutter.cut_at_uv_discontinuity());

