import site
site_location = site.getsitepackages();
site_location.append(site.getusersitepackages());
site_location.append("${CMAKE_INSTALL_PREFIX}");

import sys
import os 
root_dir = os.environ["PYMESH_PATH"];
package_dir = os.path.join(root_dir, "python/pymesh");
exec(open(os.path.join(package_dir, 'version.py')).read())
py_version = sys.version_info;
install_path = "pymesh-{}-py{}.{}.egg/pymesh/lib".format(
        __version__, py_version[0], py_version[1]);
site_location = [os.path.join(loc, install_path) for loc in site_location];

with open(os.path.join(root_dir, "cmake/SetInstallRpath.cmake"), 'w') as fout:
    fout.write("SET(CMAKE_INSTALL_RPATH \"{}\")\n".format(";".join(site_location)));
    fout.write("MESSAGE(STATUS \"CMAKE_INSTALL_RPATH: ${CMAKE_INSTALL_RPATH}\")\n");


