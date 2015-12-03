import unittest

import os.path
from .meshio import load_mesh, form_mesh, save_mesh, save_mesh_raw
import numpy.testing
import tempfile

class TestCase(unittest.TestCase):
    """ Base class for all unit tests in PyMesh.

    This class provides a list of helper methods that will come in handy during
    unit testings.  In particular, it takes care of loading test data from the
    test directory and writing test data into a temp directory that is system
    independent.

    Attributes:
        pymesh_path: Path to the root directory of the project.
        tmp_dir: Temp data directory for dumping temporary results.  In contrast
            to just using `/tmp`, `tmp_dir` is system independent and works
            even under Windows.
    """

    def setUp(self):
        """ Initialize data and tmp directories. """
        cur_path = os.path.dirname(__file__);
        self.pymesh_path = os.path.join(cur_path, "../../");
        self.pymesh_path = os.path.abspath(self.pymesh_path);
        self.tmp_dir = tempfile.gettempdir();

        self.assertTrue(os.path.exists(self.pymesh_path));

    def load_mesh(self, filename):
        """ Load mesh from file.

        Args:
            filename: file to be loaded.
                If `filename` is absolute, that file is loaded.
                If `filename` is relative, it must be relative to the test data
                    directory (`$PYMESH_PATH/tests/data/`).

        Returns:
            The Mesh object.

        Examples::
            class A(TestCase):
                def test_A(self):
                    # To load cube.obj from test directory.
                    mesh = self.load_mesh("cube.obj");

                    # To load cube.obj from the executable directory.
                    mesh2 = self.load_mesh(os.path.abspath("./cube.msh"));

        """
        self.assertTrue(os.path.exists(filename));
        return load_mesh(filename);

    def form_mesh(self, vertices, faces, voxels=None):
        """ Convert raw mesh data into Mesh object. """
        return form_mesh(vertices, faces, voxels);

    def save_mesh(self, filename, mesh, *attributes, **setting):
        """ Save mesh to file.

        Args:
            filename: If filename is relative, it would be place in a tmp dir.
            mesh: Mesh object.
            *attributes: The list of attribute names to be saved.
            **setting:
                ascii: Boolean indicator of whether to save with ascii encoding
                    when possible. Default is false.

        Returns:
            The absolute path to the saved file.
        """
        if not os.path.isabs(filename):
            filename = os.path.join(self.tmp_dir, filename);
        save_mesh(filename, mesh, *attributes, **setting);
        return filename;

    def save_mesh_raw(self, filename, vertices, faces, voxels=None, **setting):
        """ Save raw mesh data to file.

        Args:
            filename: same as `save_mesh`.
            vertices: mesh vertices.
            faces: mesh faces.
            voxels: mesh voxels if any, otherwise use None.
            **setting:
                ascii:: save as `save_mesh`

        Returns:
            The absolute path to the saved file.
        """
        if not os.path.isabs(filename):
            filename = os.path.join(self.tmp_dir, filename);
        save_mesh_raw(filename, vertices, faces, voxels, **setting);
        return filename;

    def assert_mesh_equal(self, mesh1, mesh2, attr_names=[]):
        self.assertEqual(mesh1.dim, mesh2.dim);
        self.assertEqual(mesh1.vertex_per_face, mesh2.vertex_per_face);
        if (mesh1.num_voxels > 0 or mesh2.num_voxels > 0):
            self.assertEqual(mesh1.vertex_per_voxel, mesh2.vertex_per_voxel);
        numpy.testing.assert_array_equal(mesh1.vertices, mesh2.vertices);
        numpy.testing.assert_array_equal(mesh1.faces, mesh2.faces);
        numpy.testing.assert_array_equal(mesh1.voxels, mesh2.voxels);

        for attr_name in attr_names:
            self.assertTrue(mesh1.has_attribute(attr_name));
            self.assertTrue(mesh2.has_attribute(attr_name));
            numpy.testing.assert_array_equal(
                    mesh1.get_attribute(attr_name),
                    mesh2.get_attribute(attr_name));

    def assert_array_equal(self, array1, array2):
        numpy.testing.assert_array_equal(array1, array2);

    def assert_array_almost_equal(self, array1, array2, decimal=6):
        numpy.testing.assert_array_almost_equal(array1, array2, decimal);

