FROM python:3.6
WORKDIR /root/

RUN apt-get update && apt-get install -y \
gcc \
cmake \
git \
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
mkdir -p $PYMESH_PATH/third_party/build && \
mkdir -p $PYMESH_PATH/build && \
pip install -r $PYMESH_PATH/python/requirements.txt

WORKDIR $PYMESH_PATH/third_party/build
RUN cmake .. && make && make install && make clean
WORKDIR $PYMESH_PATH/build
RUN cmake .. && make && make tools && make all_tests
WORKDIR $PYMESH_PATH
RUN ./setup.py build && ./setup.py install && rm -rf build third_party/build && python -c "import pymesh; pymesh.test()"
WORKDIR /root/
RUN rm -rf PyMesh
