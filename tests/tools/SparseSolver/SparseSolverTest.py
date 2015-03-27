# This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou #
import numpy as np
from numpy.linalg import norm
import scipy.sparse
import os
import os.path
import scipy.sparse
import sys
import unittest

PYMESH_PATH = os.environ.get("PYMESH_PATH");
if PYMESH_PATH is None:
    raise ImportError("Please set PYMESH_PATH to the correct lib path.")
sys.path.append(os.path.join(PYMESH_PATH, "lib"));
sys.path.append(os.path.join(PYMESH_PATH, "swig"));

import PyEigenUtils

class SparseSolverTest(unittest.TestCase):
    def setUp(self):
        self.size = 1024*2;
        self.matrix = scipy.sparse.diags(np.arange(self.size) + 1, 0);
        self.rhs = np.arange(self.size) + 1;

    def test_LDLT(self):
        factory = PyEigenUtils.SparseSolverFactory("LDLT");
        solver = factory.create();
        solver.analyze_pattern(self.matrix);
        solver.factorize(self.matrix);
        x = solver.solve(self.rhs);
        self.assertAlmostEqual(0.0, norm(x - np.ones(self.size)));

    #@unittest.skip("debug, LLT caused a segmentation fault")
    def test_LLT(self):
        factory = PyEigenUtils.SparseSolverFactory("LLT");
        solver = factory.create();
        solver.analyze_pattern(self.matrix);
        solver.factorize(self.matrix);
        x = solver.solve(self.rhs);
        self.assertAlmostEqual(0.0, norm(x - np.ones(self.size)));

    #@unittest.skip("debug, CG caused a segmentation fault")
    def test_CG(self):
        factory = PyEigenUtils.SparseSolverFactory("CG");
        solver = factory.create();
        solver.analyze_pattern(self.matrix);
        solver.factorize(self.matrix);
        x = solver.solve(self.rhs);
        #self.assertAlmostEqual(0.0, norm(x - np.ones(self.size)));

    @unittest.skip("debug")
    def test_SparseLU(self):
        factory = PyEigenUtils.SparseSolverFactory("SparseLU");
        solver = factory.create();
        solver.analyze_pattern(self.matrix);
        solver.factorize(self.matrix);
        x = solver.solve(self.rhs);
        self.assertAlmostEqual(0.0, norm(x - np.ones(self.size)));

    @unittest.skip("debug")
    def test_UmfPackLU(self):
        factory = PyEigenUtils.SparseSolverFactory("UmfPackLU");
        solver = factory.create();
        solver.analyze_pattern(self.matrix);
        solver.factorize(self.matrix);
        x = solver.solve(self.rhs);
        self.assertAlmostEqual(0.0, norm(x - np.ones(self.size)));

