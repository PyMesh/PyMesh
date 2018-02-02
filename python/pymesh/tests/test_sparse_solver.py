import pymesh
from pymesh.TestCase import TestCase

import numpy as np
import numpy.linalg
import scipy.sparse

class SparseSolverTest(TestCase):
    def test_identity(self):
        M = scipy.sparse.eye(100).tocsc();
        rhs = np.ones(100);
        for solver_type in pymesh.SparseSolver.get_supported_solvers():
            # The following solvers seg faults.  Debug needed.
            if solver_type == "CG":
                continue;
            if solver_type == "LSCG":
                continue;
            if solver_type == "BiCG":
                continue;
            if solver_type == "UmfPackLU":
                continue;
            solver = pymesh.SparseSolver.create(solver_type);
            solver.analyze_pattern(M);
            solver.factorize(M);
            x = solver.solve(rhs);
            self.assert_array_almost_equal(rhs.ravel(), x.ravel());

