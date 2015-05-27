import PyMeshSetting

from timethis import timethis

from numpy.testing import Tester
test = Tester().test

from Mesh import Mesh
from boolean import boolean
from selfintersection import resolve_self_intersection
from selfintersection import detect_self_intersection

__all__ = [
        "Mesh",
        "boolean",
        "resolve_self_intersection",
        "detect_self_intersection"
        ];
