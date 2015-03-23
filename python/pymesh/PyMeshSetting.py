import sys
import os.path

package_dir = os.path.abspath(os.path.dirname(__file__));
sys.path.append(os.path.join(package_dir, "lib"));
sys.path.append(os.path.join(package_dir, "swig"));

