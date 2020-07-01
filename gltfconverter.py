# Sam Kellman-Wanzer
# File converter that takes a gltf file and converts it back to 
# A graphical file format supported by PyMesh

import pygltflib
from pygltflib import *
import base64
from struct import *
import pymesh
import numpy as np

gltf = pygltflib.GLTF2.load_json("cube.gltf")

attributes = gltf.meshes[0].primitives[0].attributes

a_position = gltf.accessors[attributes.POSITION]

a_indices = gltf.accessors[gltf.meshes[0].primitives[0].indices]

bv_position = gltf.bufferViews[a_position.bufferView]

bv_indices = gltf.bufferViews[a_indices.bufferView]

b_position = gltf.buffers[bv_position.buffer]

b_indices = gltf.buffers[bv_indices.buffer]

magic = 'data:application/octet-stream;base64,'
data_position = base64.b64decode(b_position.uri[len(magic):])
data_indices = base64.b64decode(b_indices.uri[len(magic):])

c_vertices = unpack('f'*int(len(data_position)/4), data_position)

c_indices = unpack('H'*int(len(data_indices)/2), data_indices)

f_vertices = np.asarray(c_vertices)
f_faces = np.asarray(c_indices)

mesh = pymesh.form_mesh(f_vertices.reshape(-3,3), f_faces.reshape(-3,3))


print("enter the name of the file to save")

filestring = str(input())
pymesh.save_mesh(filestring, mesh)
