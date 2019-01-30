#!/usr/bin/env bash

export DIST_DIR="dist"
export PYMESH_IMAGE="pymesh/pymesh"
export TAG="py3.6"

mkdir -p $DIST_DIR
docker run --rm -v `pwd`/$DIST_DIR:/mnt $PYMESH_IMAGE:$TAG bash -c "cp /root/PyMesh/dist/*.whl /mnt/; cp /root/PyMesh/python/requirements.txt /mnt/"
docker build -t $PYMESH_IMAGE:$TAG-slim .
