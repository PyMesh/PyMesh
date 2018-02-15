FROM python:3.6
WORKDIR /root/

RUN echo "deb http://ftp.us.debian.org/debian unstable main contrib non-free" >> /etc/apt/sources.list.d/unstable.list &&\
apt-get update && apt-get install -y \
libgmp-dev \
libmpfr-dev \
libgmpxx4ldbl \
libboost-dev \
libboost-thread-dev \
&& apt-get clean

COPY dist/*.whl /root/
COPY dist/requirements.txt /root/
RUN pip install --no-cache-dir -r /root/requirements.txt && \
rm /root/requirements.txt
RUN pip install --no-cache-dir /root/*.whl && \
rm /root/*.whl
