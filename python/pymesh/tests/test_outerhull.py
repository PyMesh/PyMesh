from pymesh.TestCase import TestCase
from pymesh import compute_outer_hull
from pymesh.meshio import form_mesh
from pymesh.meshutils import generate_box_mesh
from pymesh.meshutils import merge_meshes
from pymesh.misc import Quaternion

import numpy as np
import math
import unittest

class OuterHullTest(TestCase):
    def assert_valid_attributes(self, mesh, outer_hull):
        self.assertTrue(outer_hull.has_attribute("flipped"));
        self.assertTrue(outer_hull.has_attribute("face_sources"));

        flipped = outer_hull.get_attribute("flipped");
        face_sources = outer_hull.get_attribute("face_sources");

        self.assertEqual(outer_hull.num_faces, len(flipped));
        self.assertEqual(outer_hull.num_faces, len(face_sources));
        self.assertTrue(np.all(face_sources >= 0));
        self.assertTrue(np.all(face_sources < mesh.num_faces));

    def test_simple_cube(self):
        mesh = generate_box_mesh(
                np.array([0, 0, 0]), np.array([1, 1, 1]));
        outer_hulls = compute_outer_hull(mesh, all_layers=True);
        self.assertEqual(1, len(outer_hulls));
        outer_hull = outer_hulls[0];

        self.assertTrue(outer_hull.is_closed());
        self.assertEqual(mesh.num_vertices, outer_hull.num_vertices);
        self.assertEqual(mesh.num_faces, outer_hull.num_faces);
        self.assert_valid_attributes(mesh, outer_hull);

    def test_intersecting_cubes(self):
        mesh_1 = generate_box_mesh(
                np.array([0, 0, 0]), np.array([2, 2, 2]));
        mesh_2 = generate_box_mesh(
                np.array([1, 1, 1]), np.array([3, 3, 3]));
        mesh = merge_meshes((mesh_1, mesh_2));
        outer_hull = compute_outer_hull(mesh);
        self.assertTrue(outer_hull.is_closed());
        self.assert_valid_attributes(mesh, outer_hull);

    def test_nested_cubes(self):
        mesh_1 = generate_box_mesh(
                np.array([0, 0, 0]), np.array([3, 3, 3]));
        mesh_2 = generate_box_mesh(
                np.array([1, 1, 1]), np.array([2, 2, 2]));

        mesh = merge_meshes((mesh_1, mesh_2));
        outer_hulls = compute_outer_hull(mesh, all_layers=True);
        self.assertEqual(2, len(outer_hulls));
        outer_hull = outer_hulls[0];
        interior_mesh = outer_hulls[1];

        self.assertTrue(outer_hull.is_closed());
        self.assertEqual(1, outer_hull.num_components);
        self.assert_valid_attributes(mesh, outer_hull);

        self.assertEqual(8, interior_mesh.num_vertices);
        self.assert_array_equal(([1, 1, 1], [2, 2, 2]),
                interior_mesh.bbox);

    def test_multiple_components(self):
        mesh_1 = generate_box_mesh(
                np.array([0, 0, 0]), np.array([1, 1, 1]));
        mesh_2 = generate_box_mesh(
                np.array([2, 2, 2]), np.array([3, 3, 3]));

        mesh = merge_meshes((mesh_1, mesh_2));
        outer_hulls = compute_outer_hull(mesh, all_layers=True);
        self.assertEqual(1, len(outer_hulls));
        outer_hull = outer_hulls[0];

        self.assertTrue(outer_hull.is_closed());
        self.assertEqual(2, outer_hull.num_components);
        self.assert_valid_attributes(mesh, outer_hull);

    def test_face_face_touch(self):
        mesh_1 = generate_box_mesh(
                np.array([0, 0, 0]), np.array([1, 1, 1]));
        mesh_2 = generate_box_mesh(
                np.array([0, 0, 1]), np.array([1, 1, 2]));

        mesh = merge_meshes((mesh_1, mesh_2));
        outer_hulls = compute_outer_hull(mesh, all_layers=True);
        self.assertEqual(2, len(outer_hulls));
        outer_hull = outer_hulls[0];
        interior_mesh = outer_hulls[1];

        self.assertTrue(outer_hull.is_closed());
        self.assertEqual(1, outer_hull.num_components);
        self.assert_valid_attributes(mesh, outer_hull);
        self.assert_array_equal(([0, 0, 0], [1, 1, 2]), outer_hull.bbox);

        self.assertTrue(interior_mesh.is_closed());
        self.assertEqual(1, interior_mesh.num_components);
        self.assert_array_equal(([0, 0, 1], [1, 1, 1]), interior_mesh.bbox);


