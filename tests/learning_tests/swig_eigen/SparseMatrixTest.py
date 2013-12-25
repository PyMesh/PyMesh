#!/usr/bin/env python

import numpy as np
import os
import os.path
import sys
import unittest

PYMESH_PATH = os.environ.get("PYMESH_PATH");
if PYMESH_PATH is None:
    raise ImportError("Please set PYMESH_PATH to the correct lib path.")
sys.path.append(os.path.join(PYMESH_PATH, "lib"));
sys.path.append(os.path.join(PYMESH_PATH, "swig"));

from PySwigEigen import ZSparseMatrixTest

class SparseMatrixTest(unittest.TestCase):
    def setUp(self):
        test_instance = ZSparseMatrixTest();
        self.matrix = test_instance.get_matrix();

    def test_dimension(self):
        rows = self.matrix.num_rows();
        cols = self.matrix.num_cols();
        self.assertEqual(6, rows);
        self.assertEqual(6, cols);

    def test_compress_column_format(self):
        self.assertEqual(7, self.matrix.get_outer_size());
        self.assertEqual(6, self.matrix.get_inner_size());
        self.assertEqual(6, self.matrix.get_value_size());

    def test_non_zeros_entries(self):
        inner_idx = np.zeros(self.matrix.get_inner_size(), dtype=np.intc);
        outer_idx = np.zeros(self.matrix.get_outer_size(), dtype=np.intc);
        values    = np.zeros(self.matrix.get_value_size(), dtype=float);

        self.matrix.get_inner_indices(inner_idx);
        self.matrix.get_outer_indices(outer_idx);
        self.matrix.get_values(values);

        self.assertListEqual(range(7), list(outer_idx));
        self.assertListEqual(range(6), list(inner_idx));
        self.assertListEqual(range(6), list(values));
