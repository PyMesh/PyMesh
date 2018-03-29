[![CircleCI](https://circleci.com/gh/qnzhou/PyMesh/tree/master.svg?style=svg)](https://circleci.com/gh/qnzhou/PyMesh/tree/master)
[![Build Status](https://travis-ci.org/qnzhou/PyMesh.svg?branch=master)](https://travis-ci.org/qnzhou/PyMesh)
[![Documentation Status](https://readthedocs.org/projects/pymesh/badge/?version=latest)](http://pymesh.readthedocs.io/en/latest/?badge=latest)

### About PyMesh ###

**PyMesh** is a code base developed by Qingnan Zhou for his PhD research at New
York University.  It is a rapid prototyping platform focused on geometry
processing.  **PyMesh** is written using both C++ and python, where
computational intensive functionalities are realized in C++, and Python is used
for create minimalistic and easy to use interfaces.

![PyMesh][teaser]
(Model source: [Bust of Sappho](https://www.thingiverse.com/thing:14565))

### Documentation ###

[Latest documentation](http://pymesh.readthedocs.org/en/latest/)

### Quick try ###

Perhaps the easiest way of trying out PyMesh is through
[docker](https://www.docker.com/):

    docker run -it qnzhou/pymesh
    Python 3.6.4 (default, Feb 17 2018, 09:32:33)
    [GCC 4.9.2] on linux
    Type "help", "copyright", "credits" or "license" for more information.
    >>> import pymesh

For example, to run [meshstat.py](scripts/meshstat.py):

    docker run -it --rm -v `pwd`:/models qnzhou/pymesh meshstat.py -x /models/model.obj

This command mounts your currect working directory to the `/models` directory
in a docker container and executes the `meshstat.py` script on the mesh file
`model.obj` in the currect directory.

### Download Source ###

To retrieve the code:

    git clone https://github.com/qnzhou/PyMesh.git
    cd PyMesh
    git submodule update --init

### Dependencies ###

PyMesh depends on the following tools/libraries:

* [Python](https://www.python.org/) v3.x.
* [NumPy](http://www.numpy.org/) v1.8 or higher
* [SciPy](http://www.scipy.org/) v0.13 or higher
* [nose](http://nose.readthedocs.io/en/latest/) v1.3.7 or higher
* [Eigen](http://eigen.tuxfamily.org/index.php?title=Main_Page) v3.2 or higher

Optional library:

* [SparseHash](https://code.google.com/p/sparsehash/)
* [CGAL](https://www.cgal.org/) *
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

Let `$PYMESH_PATH` be the root directory of the repository.
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

To build the PyMesh library:

    make
    make all_tests

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

[teaser]: pymesh_teaser.jpg
