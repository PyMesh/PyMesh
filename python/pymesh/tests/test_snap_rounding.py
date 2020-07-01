from pymesh.TestCase import TestCase
from pymesh import snap_rounding

import numpy as np

class SnapRoundingTest(TestCase):
    def test_obtuse_triangle(self):
        points = np.array([
            [0.0, 0.0],
            [1.0, 0.0],
            [0.5, 0.1],
            ])
        segments = np.array([
            [0, 1],
            [1, 2],
            [2, 0]
            ])

        vertices, edges = snap_rounding(points, segments, 0.2)
        self.assertEqual(3, len(vertices))
        self.assertEqual(4, len(edges))

    def test_T_junction(self):
        points = np.array([
            [0.0, 0.0],
            [1.0, 0.0],
            [0.5, 1.0],
            [0.5, 0.0],
            ])
        segments = np.array([
            [0, 1],
            [2, 3],
            ])

        vertices, edges = snap_rounding(points, segments, 0.2)
        self.assertEqual(4, len(vertices))
        self.assertEqual(3, len(edges))

    def test_cross(self):
        points = np.array([
            [0.0, 0.0],
            [1.0, 0.0],
            [0.5, 1.0],
            [0.5,-1.0],
            ])
        segments = np.array([
            [0, 1],
            [2, 3],
            ])

        vertices, edges = snap_rounding(points, segments, 0.2)
        self.assertEqual(5, len(vertices))
        self.assertEqual(4, len(edges))

    def test_non_intersecting(self):
        points = np.array([
            [1.0, 0.0],
            [0.0, 1.0],
            [0.6, 0.6],
            [0.6, 1.7],
            ])
        segments = np.array([
            [0, 1],
            [2, 3],
            ])

        vertices, edges = snap_rounding(points, segments, 0.2)
        self.assertEqual(4, len(vertices))
        self.assertEqual(3, len(edges))

    def test_parallel_line(self):
        points = np.array([
            [0.0, 0.0],
            [0.0, 1.0],
            [0.1, 0.1],
            [0.1, 2.1],
            [0.2, 0.2],
            [0.2, 2.2],
            ])
        segments = np.array([
            [0, 1],
            [2, 3],
            [4, 5],
            ])

        vertices, edges = snap_rounding(points, segments, 0.2)
        self.assertEqual(5, len(vertices))
        self.assertEqual(4, len(edges))

    def test_degeneracy(self):
        points = np.array([
            [0.0, 0.0],
            [0.0, 0.0],
            ])
        segments = np.array([
            [0, 1],
            ])

        vertices, edges = snap_rounding(points, segments, 0.2)
        self.assertEqual(0, len(vertices))
        self.assertEqual(0, len(edges))

    def test_near_degeneracy(self):
        points = np.array([
            [0.0, 0.0],
            [0.0, 1e-12],
            ])
        segments = np.array([
            [0, 1],
            ])

        vertices, edges = snap_rounding(points, segments, 0.2)
        self.assertEqual(1, len(vertices))
        self.assertEqual(0, len(edges))
