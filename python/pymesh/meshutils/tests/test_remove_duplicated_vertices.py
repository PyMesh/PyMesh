from pymesh.meshutils import remove_duplicated_vertices_raw
from pymesh.TestCase import TestCase

import numpy as np

class RemoveDuplicatedVerticesTest(TestCase):
    def test_no_removal(self):
        vertices = np.array([
            [ 0.0, 0.0, 0.0],
            [ 1.0, 0.0, 0.0],
            [ 0.0, 1.0, 0.0],
            ], dtype=float);
        faces = np.array([
            [0, 1, 2],
            ], dtype=int);

        out_vertices, out_faces, __ = remove_duplicated_vertices_raw(
                vertices, faces);

        self.assert_array_equal(vertices, out_vertices);
        self.assert_array_equal(faces, out_faces);

    def test_simple_removal(self):
        vertices = np.array([
            [ 0.0, 0.0, 0.0],
            [ 0.0, 0.0, 0.0],
            [ 1.0, 0.0, 0.0],
            [ 0.0, 1.0, 0.0],
            ], dtype=float);
        faces = np.array([
            [0, 2, 3],
            [1, 2, 3]
            ], dtype=int);

        out_vertices, out_faces, __ = remove_duplicated_vertices_raw(
                vertices, faces);

        self.assertEqual(3, len(out_vertices));
        self.assert_array_equal([[0, 1, 2], [0, 1, 2]], out_faces);

    def test_tiny_triangle(self):
        vertices = np.array([
            [ 0.0, 0.0, 0.0],
            [ 0.1, 0.0, 0.0],
            [ 0.0, 0.1, 0.0],
            ], dtype=float);
        faces = np.array([
            [0, 1, 2],
            ], dtype=int);

        out_vertices, out_faces, __ = remove_duplicated_vertices_raw(
                vertices, faces, 1.0);

        self.assertEqual(1, len(out_vertices));
        self.assert_array_equal([[0, 0, 0]], out_faces);
