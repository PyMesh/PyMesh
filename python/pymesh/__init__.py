from .version import __version__
from . import PyMeshSetting
from .timethis import timethis

from numpy.testing import Tester
test = Tester().test

# Set default logging handler to avoid "No handler found" warnings.
import logging
try:  # Python 2.7+
    from logging import NullHandler
except ImportError:
    class NullHandler(logging.Handler):
        def emit(self, record):
            pass
logging.getLogger(__name__).addHandler(NullHandler())

from .Mesh import Mesh
from .meshio import load_mesh, form_mesh, save_mesh, save_mesh_raw
from .boolean import boolean
from .CSGTree import CSGTree
from .material import Material
from .selfintersection import resolve_self_intersection
from .selfintersection import detect_self_intersection
from .outerhull import compute_outer_hull
from .winding_number import compute_winding_number
from .meshutils import *
from .misc import *
from .predicates import orient_3D, orient_2D, in_circle, in_sphere
from .slice_mesh import slice_mesh
from .VoxelGrid import VoxelGrid

__all__ = [
        "Mesh",
        "load_mesh",
        "form_mesh",
        "save_mesh",
        "save_mesh_raw",
        "boolean",
        "Material",
        "CSGTree",
        "resolve_self_intersection",
        "detect_self_intersection",
        "compute_outer_hull",
        "timethis",
        "oriented_3D",
        "oriented_2D",
        "in_circle",
        "in_sphere",
        "slice_mesh",
        "VoxelGrid"
        ];
__all__ += meshutils.__all__;
__all__ += misc.__all__;

try:
    import PyIGL
except ImportError:
    # PyIGL is missing.
    pass;
else:
    from .matrixio import load_matrix, save_matrix
    from .minkowski_sum import minkowski_sum
    from .cell_partition import partition_into_cells
    __all__ += ["minkowski_sum", "load_matrix", "save_matrix", "partition_into_cells"];

try:
    import PyCGAL
except ImportError:
    pass;
else:
    from .aabb_tree import AABBTree, distance_to_mesh, do_intersect
    __all__ += ["AABBTree", "distance_to_mesh", "do_intersect"];

try:
    import PyTetrahedronization
except ImportError:
    pass;
else:
    from .tetrahedralize import tetrahedralize
    __all__.append("tetrahedralize");

try:
    import PyTriangle, PyTriangulation
except ImportError:
    pass;
else:
    from .triangulate import triangulate, retriangulate, retriangulate_raw
    from .triangulate import triangulate_beta
    __all__ += ["triangulate", "retriangulate", "retriangulate_raw",
            "triangulate_beta"];

try:
    import PyWires
except ImportError:
    pass;
else:
    from . import wires
    __all__.append("wires");

try:
    from .Solver import SparseSolver
    __all__.append("SparseSolver");
except ImportError:
    pass;


