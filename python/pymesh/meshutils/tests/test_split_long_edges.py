from pymesh.meshutils import split_long_edges_raw
from pymesh.TestCase import TestCase

import numpy as np
import numpy.testing
import unittest

class SplitLongEdgeTest(TestCase):
    def test_nothing_to_split(self):
        vertices = np.array([
            [0.0, 0.0, 0.0],
            [1.0, 0.0, 0.0],
            [0.0, 1.0, 0.0],
            ], dtype=float);
        faces = np.array([[0, 1, 2]], dtype=int);

        out_vertices, out_faces, __ = split_long_edges_raw(vertices, faces, 2.0);

        numpy.testing.assert_array_equal(vertices, out_vertices);
        numpy.testing.assert_array_equal(faces, out_faces);

    def test_simple_split(self):
        vertices = np.array([
            [0.0, 0.0, 0.0],
            [1.0, 0.0, 0.0],
            [0.0, 1.0, 0.0],
            ], dtype=float);
        faces = np.array([[0, 1, 2]], dtype=int);

        out_vertices, out_faces, __ = split_long_edges_raw(vertices, faces, 1.4);

        self.assertEqual(2, len(out_faces));

    def test_multiple_splits(self):
        vertices = np.array([
            [0.0, 0.0, 0.0],
            [1.0, 0.0, 0.0],
            [0.0, 1.0, 0.0],
            ], dtype=float);
        faces = np.array([[0, 1, 2]], dtype=int);

        out_vertices, out_faces, __ = split_long_edges_raw(vertices, faces, 0.9);

        self.assertLess(2, len(out_faces));

