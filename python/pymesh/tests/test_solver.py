# TODO: re-enable once sparse solvers are stable.
#from pymesh import SparseSolver
#from pymesh.TestCase import TestCase
#
#import numpy as np
#import scipy.sparse
#
#class SparseSolverTest(TestCase):
#    def test_identity(self):
#        N = 1000;
#        I = scipy.sparse.identity(N);
#        rhs = np.arange(N);
#        solver = SparseSolver(I);
#        solver.factorize("LDLT");
#        x = solver.solve(rhs).ravel();
#        self.assert_array_almost_equal(rhs, x);
#
#    def test_multiple_rhs(self):
#        N = 1000;
#        I = scipy.sparse.identity(N);
#        rhs = np.arange(2*N).reshape((N, 2));
#        solver = SparseSolver(I);
#        solver.factorize("Cholmod");
#        x = solver.solve(rhs)
#        self.assert_array_almost_equal(rhs, x);
#
#    def test_lu(self):
#        N = 1000;
#        I = scipy.sparse.identity(N);
#        rhs = np.arange(2*N).reshape((N, 2));
#        solver = SparseSolver(I);
#        solver.factorize("UmfPackLU");
#        x = solver.solve(rhs)
#        self.assert_array_almost_equal(rhs, x);
