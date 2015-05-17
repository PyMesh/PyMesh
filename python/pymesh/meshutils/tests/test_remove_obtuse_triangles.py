from pymesh.meshutils import remove_obtuse_triangles_raw
from pymesh.TestCase import TestCase

import numpy as np
import numpy.testing
import unittest

class ObtuseTriangleRemovalTest(TestCase):
    def test_nothing_to_remove(self):
        vertices = np.array([
            [ 0.0, 0.0, 0.0],
            [ 1.0, 0.0, 0.0],
            [ 0.0, 1.0, 0.0],
            ]);

        faces = np.array([
            [0, 1, 2]
            ]);

        out_vertices, out_faces, info = remove_obtuse_triangles_raw(vertices, faces);

        numpy.testing.assert_array_equal(vertices, out_vertices);
        numpy.testing.assert_array_equal(faces, out_faces);

    def test_single_obtuse_triangle(self):
        vertices = np.array([
            [ 0.0, 0.0, 0.0],
            [ 1.0, 0.1, 0.0],
            [-1.0, 0.1, 0.0],
            ]);

        faces = np.array([
            [0, 1, 2]
            ]);

        out_vertices, out_faces, info = remove_obtuse_triangles_raw(vertices, faces);
        self.assertEqual(2, len(out_faces));

    def test_double_obtuse_triangles_1(self):
        """ Check two obtuse triangles sharing the longest edges.
        """
        vertices = np.array([
            [ 0.0, 0.0, 0.0],
            [ 1.0, 0.1, 0.0],
            [-1.0, 0.1, 0.0],
            [ 0.0, 0.2, 0.0],
            ]);

        faces = np.array([
            [0, 1, 2],
            [2, 1, 3],
            ]);

        out_vertices, out_faces, info = remove_obtuse_triangles_raw(vertices, faces);
        self.assertEqual(4, len(out_faces));

    def test_double_obtuse_triangles_2(self):
        """ Check two obtuse triangles that does not share the longest edge.
        """
        vertices = np.array([
            [ 0.0, 0.0, 0.0],
            [ 1.0, 0.1, 0.0],
            [-1.0, 0.1, 0.0],
            [ 5.0, 0.2, 0.0],
            ]);

        faces = np.array([
            [0, 1, 2],
            [2, 1, 3],
            ]);

        out_vertices, out_faces, info = remove_obtuse_triangles_raw(vertices, faces);
        self.assertLess(4, len(out_faces));
