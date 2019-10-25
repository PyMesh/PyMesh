from pymesh.meshutils import cut_mesh
from pymesh.TestCase import TestCase
from pymesh import form_mesh

import numpy as np
import numpy.testing
import unittest

class CutMeshTest(TestCase):
    def test_square(self):
        vertices = np.array([
            [ 0.0, 0.0, 0.0],
            [ 1.0, 0.0, 0.0],
            [ 0.0, 1.0, 0.0],
            [ 1.0, 1.0, 0.0],
            ], dtype=float)
        faces = np.array([
            [0, 1, 2],
            [2, 1, 3],
            ], dtype=int)

        mesh = form_mesh(vertices, faces)
        cutted_mesh = cut_mesh(mesh, [0, 1])

        self.assertEqual(6, cutted_mesh.num_vertices)
        self.assertEqual(2, cutted_mesh.num_components)

    def test_square_2(self):
        vertices = np.array([
            [ 0.0, 0.0],
            [ 1.0, 0.0],
            [ 0.0, 1.0],
            [ 1.0, 1.0],
            ], dtype=float)
        faces = np.array([
            [0, 1, 2],
            [2, 1, 3],
            ], dtype=int)

        mesh = form_mesh(vertices, faces)
        cutted_mesh = cut_mesh(mesh, [1, 1])

        self.assertEqual(4, cutted_mesh.num_vertices)
        self.assertEqual(1, cutted_mesh.num_components)
