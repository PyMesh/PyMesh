import PyIGL
import os.path

def load_matrix(filename):
    return PyIGL.load_MatrixF_DMAT(filename);

def save_matrix(filename, matrix):
    return PyIGL.save_MatrixF_DMAT(filename, matrix);
