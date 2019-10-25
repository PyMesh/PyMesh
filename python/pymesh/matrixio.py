import PyMesh
import os.path

def load_matrix(filename):
    """ Load matrix from file (assuming `.dmat`_ format).

    .. _.dmat: http://libigl.github.io/libigl/file-formats/dmat/
    """
    return PyMesh.load_matrix(filename)

def save_matrix(filename, matrix, in_ascii=False):
    """ Save matrix into file in `.dmat`_ format.

    Args:
        filename (``str``): Output file name.
        matrix (:py:class:`numpy.ndarray`): The matrix to save.
        in_ascii (`boolean`): Whether to save matrix in ASCII.  Default is
            false, which saves in binary format to save space.

    .. _.dmat: http://libigl.github.io/libigl/file-formats/dmat/
    """
    return PyMesh.save_matrix(filename, matrix, in_ascii)
