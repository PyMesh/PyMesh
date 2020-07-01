#Sam Kellman-Wanzer
#GLTF file converter
#Takes a type of file supported by PyMesh and converts to GLTF

import numpy as np
import pymesh
import base64

print("enter the file name")
filestring = str(input()) 

mesh = pymesh.load_mesh(filestring); 

vertex_data = mesh.vertices
face_data = mesh.faces

VERTICES = vertex_data.ravel().astype(np.float32) 
MAX_X = -np.inf
MAX_Y = -np.inf
MAX_Z = -np.inf
MIN_X =  np.inf
MIN_Y =  np.inf
MIN_Z =  np.inf

for v in vertex_data:

    MAX_X = max(MAX_X, v[0])
    MAX_Y = max(MAX_Y, v[1])
    MAX_Z = max(MAX_Z, v[2])
    MIN_X = min(MIN_X, v[0]) 
    MIN_Y = min(MIN_Y, v[1])
    MIN_Z = min(MIN_Z, v[2]) 


INDICES = face_data.ravel().astype(np.ushort)

MAX = -np.inf
MIN = np.inf

for i in face_data:

    MAX = max(MAX, i[0])
    MAX = max(MAX, i[1])
    MAX = max(MAX, i[2])
    MIN = min(MIN, i[0])
    MIN = min(MIN, i[1])
    MIN = min(MIN, i[2])
    
VERTICES = np.array(VERTICES, dtype=np.float32)
INDICES = np.array(INDICES, dtype = np.ushort)

HOWMANY_V = len(VERTICES) / 3 
HOWMANY_I = len(INDICES) 

HOWMANYBYTES_V = VERTICES.nbytes
HOWMANYBYTES_I = INDICES.nbytes

B64_VERTICES = base64.b64encode(VERTICES)
B64_INDICES = base64.b64encode(INDICES)

gltf = {

    "asset": {
        "version": "2.0",
        "generator": "Pymesh gltf converter"
    },

  "accessors": [
        {
            "bufferView": 0,
            "byteOffset": 0,
            "componentType": 5126,
            "count": HOWMANY_V,
            "type": "VEC3",
            "max": [MAX_X, MAX_Y, MAX_Z],
            "min": [MIN_X, MIN_Y, MIN_Z]
        },
        {
            "bufferView": 1,
            "byteOffset": 0,
            "componentType": 5123,
            "count": HOWMANY_I,
            "type": "SCALAR",
            "max": [MAX],
            "min": [MIN]
        }
    ], 

    "bufferViews": [
        {
            "buffer": 0,
            "byteOffset": 0,
            "byteLength": HOWMANYBYTES_V,
            "target": 34962
        },
        {
            "buffer": 1,
            "byteOffset": 0,
            "byteLength": HOWMANYBYTES_I,
            "target": 34963
        }
    ],
    
    "buffers": [
        {
            "uri": "data:application/octet-stream;base64,"+str(B64_VERTICES, 'utf-8'),
            "byteLength": HOWMANYBYTES_V
        },
        {
            "uri": "data:application/octet-stream;base64,"+str(B64_INDICES, 'utf-8'),
            "byteLength": HOWMANYBYTES_I
        }
    ],
  
    "meshes": [
        {
            "primitives": [{
                 "mode": 4,
                 "attributes": {
                     "POSITION": 0
                 },
                 "indices": 1
            }]
        }
    ],

    "nodes": [
        {
            "mesh": 0
        }
    ],

    "scenes": [
        {
            "nodes": [
                0
            ]
        }
    ],

    "scene": 0
}

print ( str(gltf).replace("'", '"') ) # we need double quotes instead of single quotes

#save file

with open('cube.gltf', 'w') as f:
    f.write(str(gltf).replace("'", '"'))
