FROM python:3.6
WORKDIR /root/

COPY dist/*.whl /root/
COPY dist/requirements.txt /root/
RUN echo "deb http://ftp.us.debian.org/debian unstable main contrib non-free" >> /etc/apt/sources.list.d/unstable.list &&\
    apt-get update && apt-get install -y libc6 && \
    pip install --no-cache-dir -r /root/requirements.txt && \
    rm /root/requirements.txt && \
    pip install --no-cache-dir /root/*.whl && \
    rm /root/*.whl
