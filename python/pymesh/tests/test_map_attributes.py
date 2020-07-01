import pymesh
from pymesh.TestCase import TestCase
import numpy as np
import numpy.linalg

class MapAttributesTest(TestCase):
    def test_map_vertex_attributes_sphere(self):
        """ Map vertex attribute between two spheres.
        """
        mesh1 = pymesh.generate_icosphere(2.0, [0.0, 0.0, 0.0], 2)
        mesh2 = pymesh.generate_icosphere(2.0, [0.0, 0.0, 0.0], 3)

        mesh1.add_attribute("x")
        mesh1.set_attribute("x", mesh1.vertices[:,0])
        pymesh.map_vertex_attribute(mesh1, mesh2, "x")

        self.assertTrue(mesh2.has_attribute("x"))

        x = mesh2.get_vertex_attribute("x").ravel()
        ground_truth = mesh2.vertices[:,0]
        residual = numpy.linalg.norm(ground_truth-x)**2 / mesh2.num_vertices
        self.assertLess(residual, 1e-2)

    def test_map_vertex_attributes_sphere_box(self):
        """ Map vertex attribute from sphere to box.
        """
        mesh1 = pymesh.generate_icosphere(2.0, [0.0, 0.0, 0.0], 2)
        mesh2 = pymesh.generate_box_mesh(np.ones(3)-2, np.ones(3), 10)

        Z = np.array([0, 0, 1])
        theta = np.arctan2(np.dot(mesh1.vertices, Z),
                numpy.linalg.norm(np.cross(mesh1.vertices, Z), axis=1))

        mesh1.add_attribute("theta")
        mesh1.set_attribute("theta", theta)
        pymesh.map_vertex_attribute(mesh1, mesh2, "theta")

        self.assertTrue(mesh2.has_attribute("theta"))

        ground_truth = np.arctan2(np.dot(mesh2.vertices, Z),
                numpy.linalg.norm(np.cross(mesh2.vertices, Z), axis=1))
        theta2 = mesh2.get_vertex_attribute("theta").ravel()

        residual = numpy.linalg.norm(ground_truth-theta2)**2 / mesh2.num_vertices
        self.assertLess(residual, 1e-2)

    def test_map_face_attribute_sphere(self):
        mesh1 = pymesh.generate_icosphere(2.0, [0.0, 0.0, 0.0], 2)
        mesh2 = pymesh.generate_icosphere(2.1, [0.0, 0.0, 0.0], 2)

        mesh1.add_attribute("face_centroid")
        centroids1 = mesh1.get_face_attribute("face_centroid")
        mesh2.add_attribute("face_centroid")
        centroids2 = mesh2.get_face_attribute("face_centroid")

        value = np.argmax(np.absolute(centroids1), axis=1)
        mesh1.add_attribute("value")
        mesh1.set_attribute("value", value)
        pymesh.map_face_attribute(mesh1, mesh2, "value")

        self.assertTrue(mesh2.has_attribute("value"))
        mapped_value = mesh2.get_face_attribute("value").ravel()

        self.assert_array_equal(value, mapped_value)

    def test_map_corner_attribute_sphere(self):
        mesh1 = pymesh.generate_icosphere(2.0, [0.0, 0.0, 0.0], 2)
        mesh2 = pymesh.generate_icosphere(2.1, [0.0, 0.0, 0.0], 2)

        value = np.arange(mesh1.num_faces * mesh1.vertex_per_face)
        mesh1.add_attribute("test")
        mesh1.set_attribute("test", value)

        pymesh.map_corner_attribute(mesh1, mesh2, "test")

        self.assertTrue(mesh2.has_attribute("test"))
        mapped_value = mesh2.get_attribute("test")

        diff = value - mapped_value
        largest_error = np.amax(np.absolute(diff))
        self.assertLess(largest_error, 1e-12)

