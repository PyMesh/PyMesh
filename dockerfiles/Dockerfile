FROM phusion/baseimage:0.9.22

# Get Ubuntu updates
USER root
RUN apt-get update && \
    apt-get upgrade -y -o Dpkg::Options::="--force-confold" && \
    apt-get -y install locales sudo && \
    echo "C.UTF-8 UTF-8" > /etc/locale.gen && \
    locale-gen && \
    rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*

# Set locale environment
ENV LC_ALL=C.UTF-8 \
    LANG=C.UTF-8 \
    LANGUAGE=C.UTF-8

COPY set-home-permissions.sh /etc/my_init.d/set-home-permissions.sh

# Set up user so that we do not run as root
# See https://github.com/phusion/baseimage-docker/issues/186
# Disable forward logging
# Add script to set up permissions of home directory on myinit
# Run ldconfig so that /usr/local/lib is in the default search
# path for the dynamic linker.
RUN useradd -m -s /bin/bash -G sudo,docker_env pymesh && \
    echo "pymesh:docker" | chpasswd && \
    echo "pymesh ALL=(ALL) NOPASSWD: ALL" >> /etc/sudoers && \
    touch /etc/service/syslog-forwarder/down && \
    chmod +x /etc/my_init.d/set-home-permissions.sh && \
    ldconfig

# Bug fix for Debian python3 site path issue
# https://bitbucket.org/fenics-project/dolfin/issues/787/debian-ubuntu-python-bug-leads-to-python3
# https://bugs.launchpad.net/ubuntu/+source/python3.5/+bug/1408092?comments=all
RUN echo "/usr/local/lib/python3/dist-packages" >> /usr/local/lib/python3.5/dist-packages/debian-ubuntu-sitepath-fix.pth

# Turn off forwarding of syslog messages to stdout
# https://github.com/phusion/baseimage-docker/issues/186
RUN touch /etc/service/syslog-forwarder/down

USER pymesh
ENV PYMESH_HOME /home/pymesh
RUN touch $PYMESH_HOME/.sudo_as_admin_successful && \
    mkdir $PYMESH_HOME/shared
VOLUME /home/pymesh/shared

# Print something nice on entry.

WORKDIR /home/pymesh
USER root
ENTRYPOINT ["/sbin/my_init","--quiet","--","/sbin/setuser","pymesh","/bin/bash","-l","-c"]
CMD ["/bin/bash","-i"]

RUN apt-get -qq update && \
    apt-get -y --with-new-pkgs \
        -o Dpkg::Options::="--force-confold" upgrade && \
    apt-get -y install curl && \
    curl -s https://packagecloud.io/install/repositories/github/git-lfs/script.deb.sh | bash && \
    apt-get -y install \
        bison \
        ccache \
        cmake \
        doxygen \
        flex \
        g++ \
        gfortran \
        git \
        git-lfs \
        graphviz \
        libboost-filesystem-dev \
        libboost-iostreams-dev \
        libboost-math-dev \
        libboost-program-options-dev \
        libboost-system-dev \
        libboost-thread-dev \
        libboost-timer-dev \
        libeigen3-dev \
        libfreetype6-dev \
        liblapack-dev \
        libmpich-dev \
        libopenblas-dev \
        libpcre3-dev \
        libpng12-dev \
        libhdf5-mpich-dev \
        libgmp-dev \
        libcln-dev \
        libmpfr-dev \
        libgmp-dev \
        libgmpxx4ldbl \
        man \
        mpich \
        nano \
        pkg-config \
        wget \
        bash-completion && \
    git lfs install && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*

# Install Python2/3 based environment
RUN apt-get -qq update && \
    apt-get -y --with-new-pkgs \
        -o Dpkg::Options::="--force-confold" upgrade && \
    apt-get -y install \
        python3-dev \
        python3-flufl.lock \
        python3-numpy \
        python3-ply \
        python3-pytest \
        python3-scipy \
        python3-six \
        python-subprocess32 \
        python-urllib3  python3-urllib3 && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*
WORKDIR /tmp
# Install setuptools
RUN wget https://bootstrap.pypa.io/get-pip.py && \
    python3 get-pip.py && \
    pip3 install --no-cache-dir setuptools && \
    rm -rf /tmp/*

ENV PYBIND11_VERSION 2.2.1
RUN pip3 install --no-cache-dir jupyter && \
    pip3 install --no-cache-dir sympy && \
    wget -nc --quiet https://github.com/pybind/pybind11/archive/v${PYBIND11_VERSION}.tar.gz && \
    tar -xf v${PYBIND11_VERSION}.tar.gz && \
    cd pybind11-${PYBIND11_VERSION} && \
    mkdir build && \
    cd build && \
    cmake -DPYBIND11_TEST=False ../ && \
    make && \
    make install && \
    rm -rf /tmp/*


WORKDIR /home/pymesh
RUN git clone https://github.com/qnzhou/PyMesh.git
ENV PYMESH_PATH /home/pymesh/PyMesh
WORKDIR $PYMESH_PATH

RUN git submodule update --init && \
rm -rf $PYMESH_PATH/third_party/build && \
rm -rf $PYMESH_PATH/build && \
mkdir -p $PYMESH_PATH/third_party/build && \
mkdir -p $PYMESH_PATH/build && \
pip3 install -r $PYMESH_PATH/python/requirements.txt && \
update-alternatives --install /usr/bin/python python /usr/bin/python3.5 2 &&\
sed  -i '/convex_hull/d' ./setup.py 
RUN python setup.py build && \
	python setup.py install 
#RUN ./setup.py build && ./setup.py install && rm -rf build third_party/build

USER pymesh
RUN cd /home/pymesh &&
    cp -r PyMesh/scripts /home/pymesh/demos

