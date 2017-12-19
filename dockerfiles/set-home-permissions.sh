#!/bin/bash
if [ "$HOST_UID" ]; then
	    usermod -u $HOST_UID pymesh
fi
if [ "$HOST_GID" ]; then
	groupmod -g $HOST_GID pymesh
fi
# This makes sure that all files in /home/fenics are accessible by the user
# fenics. We exclude the folder ~/shared to reduce IO out to the host. Docker
# for Mac, Docker for Windows and the UID/GID trick above should mean that file
# permissions work seamlessly now.
find /home/pymesh -maxdepth 1 | sed "1d" | grep -v "/home/pymesh/shared" | xargs chown -R pymesh:pymesh 2> /dev/null || true
