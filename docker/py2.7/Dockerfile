FROM python:2.7
WORKDIR /root/

RUN echo "deb http://ftp.us.debian.org/debian unstable main contrib non-free" >> /etc/apt/sources.list.d/unstable.list &&\
apt-get update && apt-get install -y \
gcc \
g++ \
git \
cmake \
libgmp-dev \
libmpfr-dev \
libgmpxx4ldbl \
libboost-dev \
libboost-thread-dev \
libtbb-dev \
&& apt-get clean

RUN git clone https://github.com/PyMesh/PyMesh.git
ENV PYMESH_PATH /root/PyMesh
WORKDIR $PYMESH_PATH

RUN git submodule update --init && \
pip install -r $PYMESH_PATH/python/requirements.txt

RUN ./setup.py bdist_wheel && rm -rf build third_party/build && pip install dist/pymesh2*.whl
WORKDIR /root/
RUN python -c "import pymesh; pymesh.test()"

# Patch wheel to bring non-standard dependencies inside.
RUN python $PYMESH_PATH/docker/patches/patch_wheel.py
