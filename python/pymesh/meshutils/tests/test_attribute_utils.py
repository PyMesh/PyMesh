from pymesh import generate_box_mesh
from pymesh import form_mesh
from pymesh import convert_to_vertex_attribute
from pymesh import convert_to_face_attribute
from pymesh import convert_to_voxel_attribute
from pymesh.TestCase import TestCase

import numpy as np

class AttributeUtilsTest(TestCase):
    def test_vertex_to_vertex(self):
        mesh = generate_box_mesh(np.zeros(3), np.ones(3));
        mesh.add_attribute("vertex_normal");
        vertex_normals = mesh.get_vertex_attribute("vertex_normal");
        vertex_normals2 = convert_to_vertex_attribute(
                mesh, vertex_normals.ravel());
        self.assert_array_equal(
                vertex_normals.ravel(),
                vertex_normals2.ravel());

    def test_face_to_vertex(self):
        mesh = generate_box_mesh(np.zeros(3), np.ones(3));
        attr = np.ones(mesh.num_faces);
        attr2 = convert_to_vertex_attribute(mesh, attr).ravel();
        self.assert_array_equal(np.ones(mesh.num_vertices), attr2);

    def test_vertex_to_face(self):
        mesh = generate_box_mesh(np.zeros(3), np.ones(3));
        attr = np.ones(mesh.num_vertices);
        attr2 = convert_to_face_attribute(mesh, attr).ravel();
        self.assert_array_equal(np.ones(mesh.num_faces), attr2);

    def test_vertex_to_voxel(self):
        vertices = np.array([
            [0.0, 0.0, 0.0],
            [1.0, 0.0, 0.0],
            [0.0, 1.0, 0.0],
            [0.0, 0.0, 1.0],
            ]);
        voxels = np.array([[0,1,2,3]],dtype=int);

        mesh = form_mesh(vertices, np.zeros((0, 3)), voxels);
        attr = np.ones(mesh.num_vertices);
        attr2 = convert_to_voxel_attribute(mesh, attr).ravel();
        self.assert_array_equal([1], attr2);

    def test_face_to_voxel(self):
        vertices = np.array([
            [0.0, 0.0, 0.0],
            [1.0, 0.0, 0.0],
            [0.0, 1.0, 0.0],
            [0.0, 0.0, 1.0],
            ]);
        voxels = np.array([[0,1,2,3]],dtype=int);

        mesh = form_mesh(vertices, np.zeros((0, 3)), voxels);
        attr = np.ones(mesh.num_faces);
        attr2 = convert_to_voxel_attribute(mesh, attr).ravel();
        self.assert_array_equal([1], attr2);
