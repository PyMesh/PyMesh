from pymesh.meshutils import remove_isolated_vertices_raw
from pymesh.TestCase import TestCase

import numpy as np
import unittest

class RemoveIsolatedVerticesTest(TestCase):
    def test_no_isolated_vertices(self):
        vertices = np.array([
            [ 0.0, 0.0, 0.0],
            [ 1.0, 0.0, 0.0],
            [ 0.0, 1.0, 0.0],
            ]);

        faces = np.array([
            [0, 1, 2]
            ]);

        out_vertices, out_faces, info = remove_isolated_vertices_raw(vertices, faces);

        self.assert_array_equal(vertices, out_vertices);
        self.assert_array_equal(faces, out_faces);
        self.assertEqual(0, info["num_vertex_removed"]);
        self.assert_array_equal(np.arange(3), info["ori_vertex_index"]);

    def test_multiple_isolated_vertices(self):
        vertices = np.array([
            [ 0.0, 0.0, 0.0],
            [ 1.0, 0.0, 0.0],
            [ 0.0, 1.0, 0.0],
            [ 1.0, 1.0, 0.0],
            [ 0.0, 0.0, 1.0],
            ]);

        faces = np.array([
            [0, 1, 4]
            ]);

        out_vertices, out_faces, info = remove_isolated_vertices_raw(vertices, faces);

        self.assertEqual(3, len(out_vertices));
        self.assertTrue(np.all(out_faces < len(out_vertices)));
        self.assertEqual(2, info["num_vertex_removed"]);
        self.assert_array_equal([0, 1, 4], info["ori_vertex_index"]);
