Installation
============

Download the Source:
--------------------

The source code can be checked out from GitHub::

    git clone --recursive git@github.com:qnzhou/PyMesh.git

The environment variable ``PYMESH_PATH`` is used by the unit tests to locate the
testing data, so be sure to set it up::

    export PYMESH_PATH=/path/to/PyMesh/

Dependencies:
-------------

PyMesh is based on the design philosophy that one should not reinvent the wheel.
It depends a number of state-of-the-art open source libraries:

* Python_: v2.7 or higher, note that Python 3.x is not tested.
* NumPy_: v1.8 or higher
* SciPy_: v0.13 or higher
* SWIG_: v3.0.5 or higher
* Eigen_: v3.2 or higher

.. _Python: https://www.python.org
.. _NumPy: https://www.numpy.org
.. _SciPy: https://www.scipy.org
.. _SWIG: http://www.swig.org
.. _Eigen: http://eigen.tuxfamily.org

The following libraries are not required, but highly recommended.  PyMesh
provides a thin wrapper to these libraries, and without them certain
functionalities would be disabled. Most of these packages can be easily
installed using package management softwares for your OS.

* SparseHash_: is used to speed up hash grid.
* CGAL_: is needed for self-intersection, convex hull, outer hull and boolean
  computations.
* tetgen_: is needed by tetrahedronization and wire inflation.
* libigl_: is needed by outer hull, boolean computations and wire inflation.
* cork_: is used by boolean computation.
* triangle_: is used by triangulation and 2D wire inflation (See
  :doc:`triangle_compilation_note`).
* qhull_: is used for computing convex hull.
* Clipper_: is used for 2D boolean operations.
* Carve_: is used for 3D boolean operations.  Minor modification is added by me
  for linux/mac compilation.

.. _SparseHash: https://code.google.com/p/sparsehash/
.. _CGAL: https://www.cgal.org
.. _tetgen: http://wias-berlin.de/software/tetgen
.. _libigl: http://igl.ethz.ch/projects/libigl/
.. _cork: https://github.com/gilbo/cork
.. _triangle: http://www.cs.cmu.edu/~quake/triangle.html
.. _qhull: http://www.qhull.org/
.. _Clipper: http://www.angusj.com/delphi/clipper.php
.. _Carve: https://github.com/qnzhou/carve

Environment Variables:
----------------------

If any dependent libraries are not installed in the default locations, e.g.
``/usr/local`` and ``opt/local``, one needs to set certain environment variables
that help PyMesh locate the libraries.  PyMesh check the following environment
variables:

* ``EIGEN_INC``: directory containing the Eigen library.
* ``GOOGLEHASH_INCLUDES``: directory containing sparse hash.
* ``CGAL_PATH``: path to CGAL library
* ``BOOST_INC``: directory containing boost.
* ``LIBIGL_PATH``: path to libigl.
* ``CORK_PATH``: path to cork.
* ``TETGEN_PATH``: path to tetgen
* ``TRIANGLE_PATH``: path to triangle
* ``QHULL_PATH``: path to qhull
* ``CLIPPER_PATH``: path to clipper
* ``CARVE_PATH``: path to carve

Building PyMesh:
----------------

To compile the optional third party libraries::

    cd $PYMESH_PATH/third_party
    mkdir build
    cd build
    cmake ..
    make
    make install

Third party dependencies will be installed in
``$PYMESH_PATH/python/pymesh/third_party`` directory.

It is recommended to build out of source, use the following commands setup building
environment::

    cd $PYMESH_PATH
    mkdir build
    cd build
    cmake ..

To only build the C++ libraries without python bindings, change the last command
to::

    cmake -DWITHOUT_SWIG=ON ..

PyMesh consists of several modules.  The main module defines the core data
structures and is used by all other modules.  To build the main module and its
unit tests::

    make
    make src_tests

The other modules are different tools to achieve certain functionalities.
Different tools may have different dependencies.  A module will be disabled if
its dependencies are not met.  To build all tools and their unit tests::

    make tools
    make tools_tests

Another way is to build each tool separately::

    # MeshUtils tools
    make MeshUtils
    make MeshUtils_tests

    # EigenUtils tools
    make EigenUtils
    make EigenUtils_tests

    # Assembler tools
    make assembler
    make assembler_tests

    # CGAL tools
    make cgal
    make cgal_tools

    # Boolean tools
    make boolean
    make boolean_tests

    # Convex hull tools
    make convex_hull
    make convex_hull_tests

    # Envolope tools
    make envolope
    make envolope_tests

    # Outer hull tools
    make outer_hull
    make outer_hull_tests

    # SelfIntersection tools
    make self_intersection
    make self_intersection_tests

    # SparseSolver tools
    make SparseSolver
    make SparseSolver_tests

    # Tetrahedronization tools
    make tetrahedronization
    make tetrahedronization_tests

    # Wire inflation tools
    make wires
    make wires_tests

    # TetGen tools
    make tetgen
    make tetgen_tests

    # Triangle tools
    make triangle
    make triangle_tests

Make sure all unit tests are passed before using the library.  Please report
unit tests failures on github.

Install PyMesh:
---------------

The output of building PyMesh consists a set of C++ libraries and a python
module. Installing the C++ library is currently not available.  However,
installing the python package can be done::

    ./setup.py build # This an alternative way of calling cmake/make
    ./setup.py install

To check PyMesh is installed correctly, one can run the unit tests::

    python -c "import pymesh; pymesh.test()"

Once again, make sure all unit tests are passed, and report any unit test
failures.
