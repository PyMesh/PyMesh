[![CircleCI](https://circleci.com/gh/qnzhou/PyMesh/tree/master.svg?style=svg)](https://circleci.com/gh/qnzhou/PyMesh/tree/master)

### About PyMesh ###

**PyMesh** is a code base developed by Qingnan Zhou for his PhD research at New
York University.  It is a rapid prototyping platform focused on geometry
processing.  **PyMesh** is written using both C++ and python, where
computational intensive functionalities are realized in C++, and Python is used
for create minimalistic and easy to use interfaces.

### Documentation ###

[Latest documentation](http://pymesh.readthedocs.org/en/latest/)

### Download Source ###

To retrieve the code:

    git clone https://github.com/qnzhou/PyMesh.git
    git submodule update --init

Be sure to save the root directory of PyMesh in the environment variable
PYMESH_PATH:

    export PYMESH_PATH=/path/to/PyMesh/

Unit tests relies on this environment variable to load relevant libraries.

### Dependencies ###

PyMesh depends on the following tools/libraries:

* [Python](https://www.python.org/) v3.x.
* [NumPy](http://www.numpy.org/) v1.8 or higher
* [SciPy](http://www.scipy.org/) v0.13 or higher
* [SWIG](http://www.swig.org/) v3.0.5 or higher
* [Eigen](http://eigen.tuxfamily.org/index.php?title=Main_Page) v3.2 or higher

Optional library:

* [SparseHash](https://code.google.com/p/sparsehash/)
* [CGAL](https://www.cgal.org/)
* [tetgen](http://wias-berlin.de/software/tetgen/) *
* [libigl](http://igl.ethz.ch/projects/libigl/) *
* [cork](https://github.com/gilbo/cork) *
* [triangle](http://www.cs.cmu.edu/~quake/triangle.html) *
* [qhull](http://www.qhull.org/) *
* [Clipper](http://www.angusj.com/delphi/clipper.php) *
* [Carve](https://github.com/qnzhou/carve) *
* [GeoGram](http://alice.loria.fr/software/geogram/doc/html/index.html)
* [Quartet](https://github.com/crawforddoran/quartet) *

Libraries marked with `*` are included in `$PYMESH_PATH/third_party` directory.

### Environment Variables ###

If any of the dependent library is not installed in standard locations
(``/usr/local/``, ``/opt/local``), one needs to set environment variables that
point to the correct directories.  PyMesh check the following environment
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
* ``GEOGRAM_PATH``: path to GeoGram
* ``QUARTET_PATH``: path to quartet

### Build ###

The first step is to compile the optional third party dependencies:

    cd $PYMESH_PATH/third_party
    mkdir build
    cd build
    cmake ..
    make
    make install

Third party dependencies will be installed in
`$PYMESH_PATH/python/pymesh/third_party` directory.

Now we can build the main project.  It is recommended to build out of source:

    cd $PYMESH_PATH
    mkdir build
    cd build
    cmake ..

To only build the C++ libraries without the swig python modules, change the last
command to

    cmake -DWITHOUT_SWIG=ON ..

To build the main PyMesh library:

    make
    make src_tests

To build all available tools:

    make tools
    make tools_tests

Make sure all unit tests are passed before using the library.

### Install ###

The output of building PyMesh consists a set of C++ libraries and a python
module. Installing the C++ library is currently not available.  However,
installing the python package can be done:

    ./setup.py build # This an alternative way of calling cmake/make
    ./setup.py install

To check PyMesh is installed correctly, run the following python unit tests::

    python -c "import pymesh; pymesh.test()"

Once again, make sure all unit tests are passed, and report any unit test
failures.
