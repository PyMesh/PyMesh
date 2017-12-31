#!/usr/bin/env python

from distutils.command.build import build
import multiprocessing
import os
import os.path
from setuptools import setup, Distribution
from subprocess import check_call
import shutil

root_dir = os.path.abspath(os.path.dirname(__file__));
package_dir = os.path.join(root_dir, "python/pymesh");
exec(open(os.path.join(package_dir, 'version.py')).read())

num_cores = multiprocessing.cpu_count();
num_cores = max(1, num_cores-1);

class BinaryDistribution(Distribution):
    def is_pure(self):
        return False;

    def has_ext_modules(self):
        return True;

class cmake_build(build):
    """
    Python packaging system is messed up.  This class redirect python to use
    cmake for configuration and compilation of pymesh.
    """

    def cleanup(self):
        install_dir = os.path.join(root_dir, "python/pymesh/third_party/lib");
        if os.path.exists(install_dir) and os.path.isdir(install_dir):
            shutil.rmtree(os.path.join(install_dir));

        lib_dir = os.path.join(root_dir, "python/pymesh/lib");
        if os.path.exists(lib_dir) and os.path.isdir(lib_dir):
            shutil.rmtree(os.path.join(lib_dir));

        swig_dir = os.path.join(root_dir, "python/pymesh/swig");
        if os.path.exists(swig_dir) and os.path.isdir(swig_dir):
            shutil.rmtree(os.path.join(swig_dir));


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
        command = "make -j {}".format(num_cores);
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
        os.chdir(root_dir);

    def run(self):
        self.cleanup();
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
            "swig/*.py",
            "lib/*.so",
            "lib/*.dylib",
            "lib/*.dll",
            "third_party/lib/*.so",
            "third_party/lib/*.dylib",
            "third_party/lib/*.dll",
            "third_party/lib64/*.so",
            "third_party/lib64/*.dylib",
            "third_party/lib64/*.dll" ]},
        #include_package_data = True,
        cmdclass={'build': cmake_build},
        scripts=[
            "scripts/add_element_attribute.py",
            "scripts/add_index.py",
            "scripts/bbox.py",
            "scripts/box_gen.py",
            "scripts/boolean.py",
            "scripts/convex_hull.py",
            "scripts/curvature.py",
            "scripts/distortion.py",
            "scripts/dodecahedron_gen.py",
            "scripts/extract_self_intersecting_faces.py",
            "scripts/fem_check.py",
            "scripts/find_file.py",
            "scripts/fix_mesh.py",
            "scripts/geodesic.py",
            "scripts/highlight_boundary_edges.py",
            "scripts/highlight_degenerated_faces.py",
            "scripts/highlight_non_oriented_edges.py",
            "scripts/highlight_self_intersection.py",
            "scripts/highlight_zero_area_faces.py",
            "scripts/highlight_inverted_tets.py",
            "scripts/hilbert_curve_gen.py",
            "scripts/icosphere_gen.py",
            "scripts/inflate.py",
            "scripts/merge.py",
            "scripts/mesh_diff.py",
            "scripts/meshconvert.py",
            "scripts/meshstat.py",
            "scripts/mesh_to_wire.py",
            "scripts/microstructure_gen.py",
            "scripts/minkowski_sum.py",
            "scripts/outer_hull.py",
            "scripts/point_cloud.py",
            "scripts/print_utils.py",
            "scripts/quad_to_tri.py",
            "scripts/remove_degenerated_triangles.py",
            "scripts/remove_duplicated_faces.py",
            "scripts/remove_isolated_vertices.py",
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
            "scripts/tet_boundary.py",
            "scripts/tet_to_hex.py",
            "scripts/triangulate.py",
            "scripts/voxelize.py",
            ],
        url = "https://github.com/qnzhou/PyMesh",
        download_url="https://github.com/qnzhou/PyMesh/tarball/v0.1",
        distclass=BinaryDistribution,
        );
