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
from .matrixio import load_matrix, save_matrix
from .aabb_tree import AABBTree, distance_to_mesh, do_intersect
from .boolean import boolean
from .cell_partition import partition_into_cells
from .CSGTree import CSGTree
from .selfintersection import resolve_self_intersection
from .selfintersection import detect_self_intersection
from .outerhull import compute_outer_hull
from .winding_number import compute_winding_number
from .meshutils import *
from .minkowski_sum import minkowski_sum
from .misc import *
from .triangulate import triangulate, retriangulate, retriangulate_raw
from .predicates import orient_3D, orient_2D, in_circle, in_sphere
from .slice_mesh import slice_mesh
from . import wires

__all__ = [
        "Mesh",
        "load_mesh",
        "form_mesh",
        "save_mesh",
        "save_mesh_raw",
        "AABBTree",
        "distance_to_mesh",
        "do_intersect",
        "boolean",
        "partition_into_cells",
        "minkowski_sum",
        "CSGTree",
        "resolve_self_intersection",
        "detect_self_intersection",
        "compute_outer_hull",
        "timethis",
        "triangulate",
        "retriangulate",
        "retriangulate_raw",
        "oriented_3D",
        "oriented_2D",
        "in_circle",
        "in_sphere",
        "slice_mesh",
        "wires"
        ];
__all__ += meshutils.__all__;
__all__ += misc.__all__;
