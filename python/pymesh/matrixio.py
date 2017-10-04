import PyMesh
import os.path

def load_matrix(filename):
    return PyMesh.load_matrix(filename);

def save_matrix(filename, matrix, in_ascii=False):
    return PyMesh.save_matrix(filename, matrix, in_ascii);
