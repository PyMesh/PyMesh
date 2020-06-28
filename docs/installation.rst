Installation
============

Docker
------

The easiest way of using PyMesh is through
`docker <https://www.docker.com/>`_, where one can simply ``pull`` a
`prebuild image of PyMesh <https://hub.docker.com/r/pymesh/pymesh/>`_ from
dockerhub::

    $ docker run -it pymesh/pymesh
    Python 3.6.4 (default, Dec 21 2017, 01:35:12)
    [GCC 4.9.2] on linux
    Type "help", "copyright", "credits" or "license" for more information.
    >>> import pymesh
    >>>

Download the Source
-------------------

The source code can be checked out from
`GitHub <https://github.com/PyMesh/PyMesh>`_::

    git clone https://github.com/PyMesh/PyMesh.git
    cd PyMesh
    git submodule update --init
    export PYMESH_PATH=`pwd`

The rest of the document assumes PyMesh is located at ``$PYMESH_PATH``.

Dependencies
------------

PyMesh is based on the design philosophy that one should not reinvent the wheel.
It depends a number of state-of-the-art open source libraries:

System dependencies
~~~~~~~~~~~~~~~~~~~

* Python_: v2.7 or higher
* NumPy_: v1.8 or higher
* SciPy_: v0.13 or higher
* Eigen_: v3.2 or higher
* TBB_: 2018 Update 1 or later
* GMP_: v6.1 or higher
* MPFR_: v4.0 or higher
* Boost_: 1.6 or higher (thread, system)

.. _Python: https://www.python.org
.. _NumPy: https://www.numpy.org
.. _SciPy: https://www.scipy.org
.. _Eigen: http://eigen.tuxfamily.org
.. _TBB: https://www.threadingbuildingblocks.org/
.. _GMP: https://gmplib.org/
.. _MPFR: https://www.mpfr.org/
.. _Boost: https://www.boost.org/

On Linux, the system dependencies can be installed with `apt-get`::

    apt-get install \
        libeigen3-dev \
        libgmp-dev \
        libgmpxx4ldbl \
        libmpfr-dev \
        libboost-dev \
        libboost-thread-dev \
        libtbb-dev \
        python3-dev

On MacOS, the system dependencies can be installed with MacPorts_::

    port install
        python36 \
        eigen3 \
        gmp \
        mpfr \
        tbb \
        boost

.. _MacPorts: https://www.macports.org/

Python dependencies such as NumPy and SciPy can be installed using `pip`::

    pip install -r $PYMESH_PATH/python/requirements.txt

Third party dependencies
~~~~~~~~~~~~~~~~~~~~~~~~

The following third-party libraries are not required, but highly recommended in
order to use the full power of Pymesh.  PyMesh
provides a thin wrapper to these libraries, and without them certain
functionalities would be disabled. Most of these packages can be easily
installed using package management software for your OS.  A copy of these
libraries are also included in the ``third_party`` directory.

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

All third party libraries are attached to the repo as submodules.  They are
built as part of PyMesh automatically.  See `Building PyMesh`_ section for more
instructions.

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

.. _Building PyMesh:

Building PyMesh
---------------

Build with Setuptools
~~~~~~~~~~~~~~~~~~~~~

Setuptools builds both the main PyMesh module as well as all third party
dependencies. To build PyMesh::

    ./setup.py build


Build with CMake
~~~~~~~~~~~~~~~~

If you are familiar with C++ and CMake, there is an alternative way of building
PyMesh.  First compile and install all of the third party dependencies::

    cd $PYMESH_PATH/third_party
    ./build.py all

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
    make tests

PyMesh libraries are all located in ``$PYMESH_PATH/python/pymesh/lib``
directory.


Install PyMesh
~~~~~~~~~~~~~~

To install PyMesh in your system::

    ./setup.py install  # May require root privilege

Alternatively, one can install PyMesh locally::

    ./setup.py install --user


Post-installation check
~~~~~~~~~~~~~~~~~~~~~~~

To check PyMesh is installed correctly, one can run the unit tests::

    python -c "import pymesh; pymesh.test()"

Please make sure all unit tests are passed, and report any unit test
failures.

