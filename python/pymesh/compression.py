import PyMesh
import numpy as np

from .Mesh import Mesh

def compress(mesh, engine_name="draco"):
    """ Compress mesh data.

    Args:
        mesh (:class:`Mesh`): Input mesh.
        engine_name (``string``): Valid engines are:

            * ``draco``: `Google's Draco engine <https://google.github.io/draco/>`_
              [#]_

    Returns:
        A binary string representing the compressed mesh data.

    A simple usage example:

    >>> mesh = pymesh.generate_icosphere(1.0, [0, 0, 0])
    >>> data = pymesh.compress(mesh)
    >>> with open("out.drc", 'wb') as fout:
    ...     fout.write(data)

    .. [#] Draco uses lossy compression.  Both accuarcy and
       vertices/face order will be lost due to compression.  Draco only works
       with triangular mesh or point cloud.

    """
    engine = PyMesh.CompressionEngine.create(engine_name)
    data = engine.compress(mesh.raw_mesh)
    return data

def decompress(data, engine_name="draco"):
    """ Decompress mesh data.

    Args:
        data (``string``): Binary string representing compressed mesh.
        engine_name (``string``): Decompression engine name.  Valid engines are:

            * ``draco``: `Google's Draco engine <https://google.github.io/draco/>`_

    Returns:
        A mesh object encoded by the data.

    A simple usage example:

    >>> mesh = pymesh.generate_icosphere(1.0, [0, 0, 0])
    >>> data = pymesh.compress(mesh)
    >>> mesh2 = pymesh.decompress(data)
    """
    engine = PyMesh.CompressionEngine.create(engine_name)
    return Mesh(engine.decompress(data))
