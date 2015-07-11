import os
import os.path
import sys

def find_file(filename, *search_path):
    """
    Look for file in a few standard locations, return absolute path if found.
    """
    if os.path.isabs(filename):
        if os.path.exists(filename):
            return filename;
        else:
            raise IOError("{} does not exist".format(filename));

    exe_dir = sys.path[0];
    root_dir = os.environ["MICROSTRUCTURES_PATH"];
    search_path = list(search_path);
    search_path.append(exe_dir);
    search_path.append(os.getcwd());
    search_path.append(root_dir);

    for path in search_path:
        file_loc = os.path.join(path, filename);
        if os.path.exists(file_loc):
            return os.path.abspath(file_loc);

    raise IOError("{} does not exist".format(filename));
