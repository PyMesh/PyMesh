import os 
import site
import sys
root_dir = os.path.dirname(os.path.dirname(os.path.realpath(__file__)));
package_dir = os.path.join(root_dir, "python/pymesh");
exec(open(os.path.join(package_dir, 'version.py')).read())
py_version = sys.version_info;
install_path = "pymesh-{}-py{}.{}.egg/pymesh/lib".format(
        __version__, py_version[0], py_version[1]);
third_party_path = "pymesh-{}-py{}.{}.egg/pymesh/third_party/lib".format(
        __version__, py_version[0], py_version[1]);

site_location = [];
if hasattr(sys, 'real_prefix'):
    # Within virtualenv.
    pass
else:
    # System wide python.
    site_location += site.getsitepackages();
    site_location.append(site.getusersitepackages());

site_location.append("${CMAKE_INSTALL_PREFIX}");
site_location = [os.path.join(loc, install_path) for loc in site_location] +\
        [os.path.join(loc, third_party_path) for loc in site_location];
site_location.append(os.path.join(package_dir, "lib"));
site_location.append(os.path.join(package_dir, "third_party/lib"));

with open(os.path.join(root_dir, "cmake/SetInstallRpath.cmake"), 'w') as fout:
    fout.write("SET(CMAKE_INSTALL_RPATH \"{}\")\n".format(";".join(site_location)));
    fout.write("MESSAGE(STATUS \"CMAKE_INSTALL_RPATH: ${CMAKE_INSTALL_RPATH}\")\n");


