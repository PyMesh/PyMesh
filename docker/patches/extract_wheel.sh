#!/usr/bin/env bash

export DIST_DIR="dist"
export PYMESH_IMAGE="pymesh/pymesh"

mkdir -p $DIST_DIR

for TAG in "py2.7" "py3.5" "py3.6" "py3.7"
do
    docker pull $PYTHON_IMAGE:$TAG
    docker run --rm -v `pwd`/$DIST_DIR:/mnt $PYMESH_IMAGE:$TAG bash -c "cp /root/PyMesh/dist/*.whl /mnt/; cp /root/PyMesh/python/requirements.txt /mnt/"
done
