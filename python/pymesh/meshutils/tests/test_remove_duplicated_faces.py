from pymesh.meshutils import remove_duplicated_faces_raw
from pymesh.TestCase import TestCase

import numpy as np
import numpy.testing
import unittest

class RemoveDuplicatedFacesTest(TestCase):
    def test_double_faces(self):
        vertices = np.array([
            [ 0.0, 0.0, 0.0],
            [ 1.0, 0.0, 0.0],
            [ 0.0, 1.0, 0.0],
            ], dtype=float);
        faces = np.array([
            [0, 1, 2],
            [2, 1, 0],
            ], dtype=int);

        out_vertices, out_faces, info = remove_duplicated_faces_raw(vertices, faces);

        # Vertices are left alone.
        numpy.testing.assert_array_equal(vertices, out_vertices);
        self.assertEqual(0, len(out_faces));

    def test_triple_faces(self):
        vertices = np.array([
            [ 0.0, 0.0, 0.0],
            [ 1.0, 0.0, 0.0],
            [ 0.0, 1.0, 0.0],
            ], dtype=float);
        faces = np.array([
            [0, 1, 2],
            [2, 1, 0],
            [0, 2, 1],
            ], dtype=int);

        out_vertices, out_faces, info = remove_duplicated_faces_raw(vertices, faces);

        # Vertices are left alone.
        numpy.testing.assert_array_equal(vertices, out_vertices);
        self.assertEqual(1, len(out_faces));
