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
import sysconfig
root_dir = os.path.dirname(os.path.dirname(os.path.realpath(__file__)));
package_dir = os.path.join(root_dir, "python", "pymesh");
exec(open(os.path.join(package_dir, 'version.py')).read())
py_version = sys.version_info;

# Python packing is a nightmare...
install_pathes = ["pymesh", "pymesh2",
        "pymesh-{}.egg".format(__version__),
        "pymesh2-{}.egg".format(__version__),
        "pymesh-{}-py{}.{}.egg".format(__version__, py_version[0], py_version[1]),
        "pymesh2-{}-py{}.{}.egg".format(__version__, py_version[0], py_version[1]),
        "pymesh-{}-py{}.{}-{}.egg".format(__version__, py_version[0],
            py_version[1], sysconfig.get_platform()),
        "pymesh2-{}-py{}.{}-{}.egg".format(__version__, py_version[0],
            py_version[1], sysconfig.get_platform()),
        ];

install_egg_path = os.path.join("pymesh-{}-py{}.{}.egg".format(
        __version__, py_version[0], py_version[1]), "pymesh");
install_path = "pymesh";

site_location = ["${CMAKE_INSTALL_PREFIX}"];
if hasattr(sys, 'real_prefix'):
    # Within virtualenv.
    site_location.append(os.path.join(
        sys.prefix, "lib", "python{}.{}".format(py_version[0], py_version[1]), "site-packages"));
    site_location.append(os.path.join(
        sys.real_prefix, "lib", "python{}.{}".format(py_version[0], py_version[1]), "site-packages"));
else:
    # System wide python.
    site_location += site.getsitepackages();
    site_location.append(site.getusersitepackages());

site_locations = [];
for install_path in install_pathes:
    for loc in site_location:
        site_locations.append(os.path.join(loc, install_path, "lib"));
        site_locations.append(os.path.join(loc, install_path,
            "third_party", "lib"));

site_locations.append(os.path.join(package_dir, "lib"));
site_locations.append(os.path.join(package_dir, "third_party", "lib"));

if platform.system() == "Linux":
    site_locations.append("$ORIGIN");
    site_locations.append(os.path.join("$ORIGIN", "..", "third_party", "lib"));

with open(os.path.join(root_dir, "cmake", "SetInstallRpath.cmake"), 'w') as fout:
    fout.write("SET(CMAKE_INSTALL_RPATH \"{}\")\n".format(
        ";".join(site_locations).replace("\\", "/")));
    #fout.write("MESSAGE(STATUS \"CMAKE_INSTALL_RPATH: ${CMAKE_INSTALL_RPATH}\")\n");


