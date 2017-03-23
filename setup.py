#!/usr/bin/env python

from setuptools import setup
from distutils.command.build import build
from subprocess import check_call;
import os
import os.path

root_dir = os.path.abspath(os.path.dirname(__file__));
package_dir = os.path.join(root_dir, "python/pymesh");
exec(open(os.path.join(package_dir, 'version.py')).read())

class cmake_build(build):
    """
    Python packaging system is messed up.  This class redirect python to use
    cmake for configuration and compilation of pymesh.
    """

    def build_third_party(self):
        """
        Config and build third party dependencies.
        """
        build_dir = os.path.join(root_dir, "third_party/build");
        if not os.path.isdir(build_dir):
            os.makedirs(build_dir);

        os.chdir(build_dir);
        command = "cmake ..";
        check_call(command.split());
        command = "make";
        check_call(command.split());
        command = "make install";
        check_call(command.split());

    def build_pymesh(self):
        """
        Config and build pymesh.
        """
        build_dir = os.path.join(root_dir, "build");
        if not os.path.isdir(build_dir):
            os.makedirs(build_dir);

        os.chdir(build_dir);
        command = "cmake ..";
        check_call(command.split());
        command = "make";
        check_call(command.split());
        command = "make tools";
        check_call(command.split());
        os.chdir(root_dir);

    def run(self):
        self.build_third_party();
        self.build_pymesh();
        build.run(self);

setup(
        name = "pymesh2",
        description = "Mesh Processing for Python",
        version = __version__,
        author = "Qingnan Zhou",
        author_email = "qnzhou@gmail.com",
        license = "MPL",
        package_dir = {"": "python"},
        packages = ["pymesh", "pymesh.misc", "pymesh.meshutils", "pymesh.wires",
            "pymesh.tests", "pymesh.meshutils.tests", "pymesh.wires.tests"],
        package_data = {"pymesh": [
            "python/pymesh/swig/*.py",
            "python/pymesh/lib/*.so",
            "python/pymesh/lib/*.dylib",
            "python/pymesh/lib/*.dll",
            "python/pymesh/third_party/lib/*"]},
        include_package_data = True,
        cmdclass={'build': cmake_build},
        scripts=[
            "scripts/add_element_attribute.py",
            "scripts/add_index.py",
            "scripts/bbox.py",
            "scripts/box_gen.py",
            "scripts/boolean.py",
            "scripts/curvature.py",
            "scripts/dodecahedron_gen.py",
            "scripts/extract_self_intersecting_faces.py",
            "scripts/find_file.py",
            "scripts/fix_mesh.py",
            "scripts/geodesic.py",
            "scripts/highlight_boundary_edges.py",
            "scripts/highlight_degenerated_faces.py",
            "scripts/highlight_non_oriented_edges.py",
            "scripts/highlight_self_intersection.py",
            "scripts/highlight_zero_area_faces.py",
            "scripts/icosphere_gen.py",
            "scripts/inflate.py",
            "scripts/merge.py",
            "scripts/mesh_diff.py",
            "scripts/meshconvert.py",
            "scripts/meshstat.py",
            "scripts/microstructure_gen.py",
            "scripts/minkowski_sum.py",
            "scripts/outer_hull.py",
            "scripts/point_cloud.py",
            "scripts/print_utils.py",
            "scripts/quad_to_tri.py",
            "scripts/remove_degenerated_triangles.py",
            "scripts/resolve_self_intersection.py",
            "scripts/retriangulate.py",
            "scripts/rigid_transform.py",
            "scripts/scale_mesh.py",
            "scripts/self_union.py",
            "scripts/separate.py",
            "scripts/slice_mesh.py",
            "scripts/subdivide.py",
            "scripts/submesh.py",
            "scripts/tet.py",
            "scripts/tet_to_hex.py",
            "scripts/voxelize.py",
            ],
        url = "https://github.com/qnzhou/PyMesh",
        download_url="https://github.com/qnzhou/PyMesh/tarball/v0.1",
        install_requires=[
                'numpy',
                'scipy',
                'nose'
                ],
        );
