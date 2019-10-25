import pymesh
from pymesh.TestCase import TestCase

import numpy as np
import numpy.linalg
import scipy.sparse

class SparseSolverTest(TestCase):
    def test_identity(self):
        N =1000
        M = scipy.sparse.eye(N)
        rhs = np.ones(N)
        for solver_type in pymesh.SparseSolver.get_supported_solvers():
            solver = pymesh.SparseSolver.create(solver_type)
            solver.max_iterations = 10
            solver.analyze_pattern(M)
            solver.factorize(M)
            x = solver.solve(rhs)
            self.assert_array_almost_equal(rhs.ravel(), x.ravel())

