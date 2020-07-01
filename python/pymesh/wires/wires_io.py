from . import WireNetwork

def load_wires(wire_file):
    """ Create a WireNetwork object from file.
    """
    return WireNetwork.create_from_file(wire_file)

def form_wires(vertices, edges):
    """ Create a WireNetwork object from raw data.
    """
    return WireNetwork.create_from_data(vertices, edges)

def save_wires(wire_file, wire_network):
    """ Save a WireNetwork object to file.
    """
    wire_network.write_to_file(wire_file)
