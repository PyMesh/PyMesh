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
from .Assembler import Assembler
from .boolean import boolean
from .convex_hull import convex_hull
from .CSGTree import CSGTree
from .exact_arithmetic import Gmpz, Gmpq
from .HarmonicSolver import HarmonicSolver
from .material import Material
from .selfintersection import resolve_self_intersection
from .selfintersection import detect_self_intersection
from .outerhull import compute_outer_hull
from .winding_number import compute_winding_number
from .meshutils import *
from .misc import *
from .predicates import orient_3D, orient_2D, in_circle, in_sphere
from .slice_mesh import slice_mesh
from .submesh import submesh
from .VoxelGrid import VoxelGrid
from .tetrahedralize import tetrahedralize
from .matrixio import load_matrix, save_matrix
from .minkowski_sum import minkowski_sum
from .cell_partition import partition_into_cells
from .aabb_tree import AABBTree, distance_to_mesh, do_intersect
from .triangle import triangle
from .tetgen import tetgen
from .triangulate import triangulate_beta
from .wires import *
from .SparseSolver import SparseSolver

__all__ = [
        "Mesh",
        "Assembler",
        "load_mesh",
        "form_mesh",
        "save_mesh",
        "save_mesh_raw",
        "boolean",
        "CSGTree",
        "Gmpq",
        "Gmpz",
        "load_matrix",
        "Material",
        "save_matrix",
        "resolve_self_intersection",
        "detect_self_intersection",
        "compute_outer_hull",
        "compute_winding_number",
        "slice_mesh",
        "submesh",
        "timethis",
        "orient_3D",
        "orient_2D",
        "in_circle",
        "in_sphere",
        "slice_mesh",
        "triangle",
        "triangulate_beta",
        "tetgen",
        "tetrahedralize",
        "minkowski_sum",
        "partition_into_cells",
        "AABBTree",
        "distance_to_mesh",
        "do_intersect",
        "VoxelGrid",
        "HarmonicSolver",
        "SparseSolver"
        ];
__all__ += meshutils.__all__;
__all__ += misc.__all__;
__all__.append("wires");

