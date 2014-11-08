import numpy as np

from WireTest import WireTest
import PyWires

class InflatorEngineTest(WireTest):
    def assert_inflated_mesh_is_valid(self, wires, vertices, faces, face_sources):
        self.assertLess(0, len(vertices));
        self.assertLess(0, len(faces));
        self.assertEqual(len(faces), len(face_sources));
        self.assertLess(np.amax(face_sources)-1, wires.get_num_vertices());
        self.assertLess(-np.amin(face_sources)-1, wires.get_num_edges());

    def test_simple_3D(self):
        wires = self.load_wire_file("cube.wire");
        wires.scale_fit(np.ones(3) * -2.5, np.ones(3) * 2.5);
        inflator = PyWires.InflatorEngine.create("simple", wires);
        inflator.set_thickness_type(PyWires.InflatorEngine.PER_VERTEX);
        inflator.set_thickness(np.ones(wires.get_num_vertices()));
        inflator.inflate();

        vertices = inflator.get_vertices();
        faces = inflator.get_faces();
        face_sources = inflator.get_face_sources();
        self.assert_inflated_mesh_is_valid(wires, vertices, faces, face_sources);

    def test_simple_2D(self):
        wires = self.load_wire_file("square.wire");
        wires.scale_fit(np.ones(2) * -2.5, np.ones(2) * 2.5);
        inflator = PyWires.InflatorEngine.create("simple", wires);
        inflator.set_thickness_type(PyWires.InflatorEngine.PER_VERTEX);
        inflator.set_thickness(np.ones(wires.get_num_vertices()));
        inflator.inflate();

        vertices = inflator.get_vertices();
        faces = inflator.get_faces();
        face_sources = inflator.get_face_sources();
        self.assert_inflated_mesh_is_valid(wires, vertices, faces, face_sources);

    def test_periodic_3D(self):
        wires = self.load_wire_file("cube.wire");
        wires.scale_fit(np.ones(3) * -2.5, np.ones(3) * 2.5);
        inflator = PyWires.InflatorEngine.create("periodic", wires);
        inflator.set_thickness_type(PyWires.InflatorEngine.PER_VERTEX);
        inflator.set_thickness(np.ones(wires.get_num_vertices()));
        inflator.inflate();

        vertices = inflator.get_vertices();
        faces = inflator.get_faces();
        face_sources = inflator.get_face_sources();
        self.assert_inflated_mesh_is_valid(wires, vertices, faces, face_sources);

    def test_periodic_2D(self):
        wires = self.load_wire_file("square.wire");
        wires.scale_fit(np.ones(2) * -2.5, np.ones(2) * 2.5);
        inflator = PyWires.InflatorEngine.create("periodic", wires);
        inflator.set_thickness_type(PyWires.InflatorEngine.PER_VERTEX);
        inflator.set_thickness(np.ones(wires.get_num_vertices()));
        inflator.inflate();

        vertices = inflator.get_vertices();
        faces = inflator.get_faces();
        face_sources = inflator.get_face_sources();
        self.assert_inflated_mesh_is_valid(wires, vertices, faces, face_sources);

    def test_simple_per_edge_thickness_3D(self):
        wires = self.load_wire_file("star_3D.wire");
        wires.scale_fit(np.ones(3) * -2.5, np.ones(3) * 2.5);
        inflator = PyWires.InflatorEngine.create("simple", wires);
        inflator.set_thickness_type(PyWires.InflatorEngine.PER_EDGE);
        inflator.set_thickness(
                np.arange(wires.get_num_edges()) * 0.1 + 1.0);
        inflator.inflate();

        vertices = inflator.get_vertices();
        faces = inflator.get_faces();
        face_sources = inflator.get_face_sources();
        self.assert_inflated_mesh_is_valid(wires, vertices, faces, face_sources);

    def test_simple_per_edge_thickness_2D(self):
        wires = self.load_wire_file("star_2D.wire");
        wires.scale_fit(np.ones(2) * -2.5, np.ones(2) * 2.5);
        inflator = PyWires.InflatorEngine.create("simple", wires);
        inflator.set_thickness_type(PyWires.InflatorEngine.PER_EDGE);
        inflator.set_thickness(
                np.arange(wires.get_num_edges()) * 0.1 + 1.0);
        inflator.inflate();

        vertices = inflator.get_vertices();
        faces = inflator.get_faces();
        face_sources = inflator.get_face_sources();
        self.assert_inflated_mesh_is_valid(wires, vertices, faces, face_sources);

    def test_periodic_per_edge_thickness_3D(self):
        wires = self.load_wire_file("star_3D.wire");
        wires.scale_fit(np.ones(3) * -2.5, np.ones(3) * 2.5);
        inflator = PyWires.InflatorEngine.create("periodic", wires);
        inflator.set_thickness_type(PyWires.InflatorEngine.PER_EDGE);
        inflator.set_thickness(
                np.arange(wires.get_num_edges()) * 0.1 + 1.0);
        inflator.inflate();

        vertices = inflator.get_vertices();
        faces = inflator.get_faces();
        face_sources = inflator.get_face_sources();
        self.assert_inflated_mesh_is_valid(wires, vertices, faces, face_sources);

    def test_periodic_per_edge_thickness_2D(self):
        wires = self.load_wire_file("star_2D.wire");
        wires.scale_fit(np.ones(2) * -2.5, np.ones(2) * 2.5);
        inflator = PyWires.InflatorEngine.create("periodic", wires);
        inflator.set_thickness_type(PyWires.InflatorEngine.PER_EDGE);
        inflator.set_thickness(
                np.arange(wires.get_num_edges()) * 0.1 + 1.0);
        inflator.inflate();

        vertices = inflator.get_vertices();
        faces = inflator.get_faces();
        face_sources = inflator.get_face_sources();
        self.assert_inflated_mesh_is_valid(wires, vertices, faces, face_sources);

    def test_3D_with_profile(self):
        wires = self.load_wire_file("star_3D.wire");
        wires.scale_fit(np.ones(3) * -2.5, np.ones(3) * 2.5);
        inflator = PyWires.InflatorEngine.create("periodic", wires);
        inflator.set_thickness_type(PyWires.InflatorEngine.PER_EDGE);
        inflator.set_thickness(
                np.arange(wires.get_num_edges()) * 0.1 + 1.0);
        profile = PyWires.WireProfile.create("octagon");
        inflator.set_profile(profile);
        inflator.inflate();

        vertices = inflator.get_vertices();
        faces = inflator.get_faces();
        face_sources = inflator.get_face_sources();
        self.assert_inflated_mesh_is_valid(wires, vertices, faces, face_sources);

    def test_3D_with_dense_profile(self):
        wires = self.load_wire_file("star_3D.wire");
        wires.scale_fit(np.ones(3) * -2.5, np.ones(3) * 2.5);
        inflator = PyWires.InflatorEngine.create("periodic", wires);
        inflator.set_thickness_type(PyWires.InflatorEngine.PER_EDGE);
        inflator.set_thickness(
                np.arange(wires.get_num_edges()) * 0.1 + 1.0);
        profile = PyWires.WireProfile.create_isotropic(20);
        inflator.set_profile(profile);
        inflator.inflate();

        vertices = inflator.get_vertices();
        faces = inflator.get_faces();
        face_sources = inflator.get_face_sources();
        self.assert_inflated_mesh_is_valid(wires, vertices, faces, face_sources);

