FROM python:3.6
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
&& apt-get clean

RUN git clone https://github.com/qnzhou/PyMesh.git
ENV PYMESH_PATH /root/PyMesh
WORKDIR $PYMESH_PATH

RUN git submodule update --init && \
rm -rf $PYMESH_PATH/third_party/build && \
rm -rf $PYMESH_PATH/build && \
mkdir -p $PYMESH_PATH/third_party/build && \
mkdir -p $PYMESH_PATH/build && \
pip install -r $PYMESH_PATH/python/requirements.txt

RUN ./setup.py build && ./setup.py install && rm -rf build third_party/build
WORKDIR /root/
#RUN rm -rf PyMesh
RUN python -c "import pymesh; pymesh.test()"
