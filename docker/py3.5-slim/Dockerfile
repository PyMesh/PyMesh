FROM python:3.5
WORKDIR /root/

COPY dist/*.whl /root/
COPY dist/requirements.txt /root/
RUN pip install --no-cache-dir -r /root/requirements.txt && \
    rm /root/requirements.txt && \
    pip install --no-cache-dir /root/*.whl && \
    rm /root/*.whl
