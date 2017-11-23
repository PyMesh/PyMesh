"""
RPath generation.

The purpose of this script to guess where pymesh will be installed in the
system and store the install directories as RPath into the inter-linking
libraries pymesh generates.
"""

import os
import os.path
import site
import sys
import platform
root_dir = os.path.dirname(os.path.dirname(os.path.realpath(__file__)));
package_dir = os.path.join(root_dir, "python/pymesh");
exec(open(os.path.join(package_dir, 'version.py')).read())
py_version = sys.version_info;

install_egg_path = "pymesh-{}-py{}.{}.egg/pymesh".format(
        __version__, py_version[0], py_version[1]);
install_path = "pymesh";

site_location = ["${CMAKE_INSTALL_PREFIX}"];
if hasattr(sys, 'real_prefix'):
    # Within virtualenv.
    site_location.append(os.path.join(sys.prefix,
        "lib/python{}.{}/site-packages".format(py_version[0], py_version[1])));
    site_location.append(os.path.join(sys.real_prefix,
        "lib/python{}.{}/site-packages".format(py_version[0], py_version[1])));
else:
    # System wide python.
    site_location += site.getsitepackages();
    site_location.append(site.getusersitepackages());

site_location = \
        [os.path.join(loc, "{}/lib".format(install_egg_path))
                for loc in site_location] +\
        [os.path.join(loc, "{}/third_party/lib".format(install_egg_path))
                for loc in site_location] +\
        [os.path.join(loc, "{}/lib".format(install_path))
                for loc in site_location] +\
        [os.path.join(loc, "{}/third_party/lib".format(install_path))
                for loc in site_location];
site_location.append(os.path.join(package_dir, "lib"));
site_location.append(os.path.join(package_dir, "third_party/lib"));

if platform.system() == "Linux":
    site_location.append("$ORIGIN");
    site_location.append("$ORIGIN/../third_party/lib");

with open(os.path.join(root_dir, "cmake/SetInstallRpath.cmake"), 'w') as fout:
    fout.write("SET(CMAKE_INSTALL_RPATH \"{}\")\n".format(";".join(site_location)));
    fout.write("MESSAGE(STATUS \"CMAKE_INSTALL_RPATH: ${CMAKE_INSTALL_RPATH}\")\n");


