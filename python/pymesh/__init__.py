import PyMeshSetting

from timethis import timethis

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

from Mesh import Mesh
from meshio import load_mesh, form_mesh, save_mesh, save_mesh_raw
from boolean import boolean
from selfintersection import resolve_self_intersection
from selfintersection import detect_self_intersection
from outerhull import compute_outer_hull
from meshutils import *
from misc import *
from timethis import timethis

__all__ = [
        "Mesh",
        "load_mesh",
        "form_mesh",
        "save_mesh",
        "save_mesh_raw",
        "boolean",
        "resolve_self_intersection",
        "detect_self_intersection",
        "compute_outer_hull",
        "timethis",
        "wires"
        ];
__all__ += meshutils.__all__;
__all__ += misc.__all__;
