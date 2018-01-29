Installation
============

Docker
------

The easiest way of using PyMesh is through
`docker <https://www.docker.com/>`_, where one can simply ``pull`` a
`prebuild image of PyMesh <https://hub.docker.com/r/qnzhou/pymesh/>`_ from
dockerhub::

    $ docker run -it qnzhou/pymesh
    Python 3.6.4 (default, Dec 21 2017, 01:35:12)
    [GCC 4.9.2] on linux
    Type "help", "copyright", "credits" or "license" for more information.
    >>> import pymesh
    >>>

Download the Source
-------------------

The source code can be checked out from GitHub::

    git clone git@github.com:qnzhou/PyMesh.git
    cd PyMesh
    git submodule update --init

Dependencies
------------

PyMesh is based on the design philosophy that one should not reinvent the wheel.
It depends a number of state-of-the-art open source libraries:

* Python_: v2.7 or higher
* NumPy_: v1.8 or higher
* SciPy_: v0.13 or higher
* Eigen_: v3.2 or higher

.. _Python: https://www.python.org
.. _NumPy: https://www.numpy.org
.. _SciPy: https://www.scipy.org
.. _Eigen: http://eigen.tuxfamily.org

The following libraries are not required, but highly recommended.  PyMesh
provides a thin wrapper to these libraries, and without them certain
functionalities would be disabled. Most of these packages can be easily
installed using package management softwares for your OS.  A copy of these
libraries are also included in the `third_party` direcgtory.

* SparseHash_: is used to speed up hash grid.
* CGAL_: is needed for self-intersection, convex hull, outer hull and boolean
  computations.
* tetgen_: is needed by tetrahedronization and wire inflation.
* libigl_: is needed by outer hull, boolean computations and wire inflation.
* cork_: is used by boolean computation.
* triangle_: is used by triangulation and 2D wire inflation.
* qhull_: is used for computing convex hull.
* Clipper_: is used for 2D boolean operations.
* Carve_: is used for 3D boolean operations.  Minor modification is added by me
  for linux/mac compilation.
* GeoGram_: is used as a 2D triangle and 3D tetrahedron generation engine.
* Quartet_: is used as a 3D tetrahedralization engine.
* MMG3D_: is used as a 3D tetrahedralizaiton optimization engine.

.. _SparseHash: https://code.google.com/p/sparsehash/
.. _CGAL: https://www.cgal.org
.. _tetgen: http://wias-berlin.de/software/tetgen
.. _libigl: http://igl.ethz.ch/projects/libigl/
.. _cork: https://github.com/gilbo/cork
.. _triangle: http://www.cs.cmu.edu/~quake/triangle.html
.. _qhull: http://www.qhull.org/
.. _Clipper: http://www.angusj.com/delphi/clipper.php
.. _Carve: https://github.com/qnzhou/carve
.. _GeoGram: http://alice.loria.fr/software/geogram/doc/html/index.html
.. _Quartet: https://github.com/crawforddoran/quartet
.. _MMG3D: https://www.mmgtools.org/

Environment Variables
---------------------

If any dependent libraries are not installed in the default locations, e.g.
``/usr/local`` and ``opt/local``, one needs to set certain environment variables
that help PyMesh locate the libraries.  PyMesh check the following environment
variables:

* ``EIGEN_INC``: directory containing the Eigen library.
* ``GOOGLEHASH_INCLUDES``: directory containing sparse hash.
* ``CGAL_PATH``: path to CGAL library.
* ``BOOST_INC``: directory containing boost.
* ``LIBIGL_PATH``: path to libigl.
* ``CORK_PATH``: path to cork.
* ``TETGEN_PATH``: path to tetgen.
* ``TRIANGLE_PATH``: path to triangle.
* ``QHULL_PATH``: path to qhull.
* ``CLIPPER_PATH``: path to clipper.
* ``CARVE_PATH``: path to carve.
* ``GEOGRAM_PATH``: path to GeoGram.
* ``QUARTET_PATH``: path to Quartet.

Building PyMesh
---------------

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

PyMesh consists of several modules.  To build all modules and their
corresponding unit tests::

    make
    make all_tests

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

Install PyMesh
--------------

The output of building PyMesh consists a set of C++ libraries and a python
module. Installing the C++ library is currently not available.  However,
installing the python package can be done::

    ./setup.py build # This an alternative way of calling cmake/make
    ./setup.py install # This may require admin privilage.

Alternatively, one can install PyMesh locally::

    ./setup.py intall --user


To check PyMesh is installed correctly, one can run the unit tests::

    python -c "import pymesh; pymesh.test()"

Once again, make sure all unit tests are passed, and report any unit test
failures.
