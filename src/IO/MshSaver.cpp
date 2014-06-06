#include "MshSaver.h"

#include <cassert>
#include <iostream>

using namespace Zhou;

MshSaver::MshSaver(const std::string& filename, bool binary) :
    m_binary(binary), m_num_nodes(0), m_num_elements(0), m_dim(0) {
        fout.open(filename.c_str(), std::fstream::out);
        if (!fout) {
            std::cerr << "Error opening " << filename << " to write msh file." << std::endl;
            throw INVALID_FORMAT;
        }
}

MshSaver::~MshSaver() {
    fout.close();
}

void MshSaver::save_mesh(const VectorF& nodes, const VectorI& elements,
        size_t dim, size_t nodes_per_element) {
    if (nodes_per_element != 3 &&
            nodes_per_element != 4 &&
            nodes_per_element != 8) {
        std::cerr << "Error saving msh: "
            << "Only triangle, tet and hexahedron elements are supported!" << std::endl;
        throw NOT_IMPLEMENTED;
    }
    if (dim != 2 && dim != 3) {
        std::cerr << "Only support 2 or 3 dimentional mesh!" << std::endl;
        throw NOT_IMPLEMENTED;
    }
    m_dim = dim;

    if (m_binary) {
        save_binary_mesh(nodes, elements, nodes_per_element);
    } else {
        save_ascii_mesh(nodes, elements, nodes_per_element);
    }
}

void MshSaver::save_scalar_field(const std::string& fieldname, const VectorF& field) {
    assert(field.size() == m_num_nodes);
    fout << "$NodeData" << std::endl;
    fout << "1" << std::endl; // num string tags.
    fout << "\"" << fieldname << "\"" << std::endl;
    fout << "1" << std::endl; // num real tags.
    fout << "0.0" << std::endl; // time value.
    fout << "3" << std::endl; // num int tags.
    fout << "0" << std::endl; // the time step
    fout << "1" << std::endl; // 1-component scalar field.
    fout << m_num_nodes << std::endl; // number of nodes

    if (m_binary) {
        for (size_t i=0; i<m_num_nodes; i++) {
            int node_idx = i+1;
            fout.write((char*)&node_idx, sizeof(int));
            fout.write((char*)&field[i], sizeof(Float));
        }
    } else {
        for (size_t i=0; i<m_num_nodes; i++) {
            int node_idx = i+1;
            fout << node_idx << " " << field[i] << std::endl;
        }
    }
    fout << "$EndNodeData" << std::endl;
    fout.flush();
}

void MshSaver::save_vector_field(const std::string& fieldname, const VectorF& field) {
    assert(field.size() == m_dim * m_num_nodes);
    fout << "$NodeData" << std::endl;
    fout << "1" << std::endl; // num string tags.
    fout << "\"" << fieldname << "\"" << std::endl;
    fout << "1" << std::endl; // num real tags.
    fout << "0.0" << std::endl; // time value.
    fout << "3" << std::endl; // num int tags.
    fout << "0" << std::endl; // the time step
    fout << "3" << std::endl; // 3-component vector field.
    fout << m_num_nodes << std::endl; // number of nodes

    const Float zero = 0.0;
    if (m_binary) {
        for (size_t i=0; i<m_num_nodes; i++) {
            int node_idx = i+1;
            fout.write((char*)&node_idx, sizeof(int));
            if (m_dim == 3)
                fout.write((char*)&field[i*3], sizeof(Float)*3);
            else if (m_dim == 2) {
                fout.write((char*)&field[i*2], sizeof(Float)*2);
                fout.write((char*)&zero, sizeof(Float)); // Set z=0
            }
        }
    } else {
        for (size_t i=0; i<m_num_nodes; i++) {
            int node_idx = i+1;
            if (m_dim == 3) {
                fout << node_idx
                    << " " << field[i*3]
                    << " " << field[i*3+1]
                    << " " << field[i*3+2]
                    << std::endl;
            } else if (m_dim == 2) {
                fout << node_idx
                    << " " << field[i*2]
                    << " " << field[i*2+1]
                    << " " << zero
                    << std::endl;
            }
        }
    }
    fout << "$EndNodeData" << std::endl;
    fout.flush();
}

void MshSaver::save_elem_scalar_field(const std::string& fieldname, const VectorF& field) {
    assert(field.size() == m_num_elements);
    fout << "$ElementData" << std::endl;
    fout << 1 << std::endl; // num string tags.
    fout << "\"" << fieldname << "\"" << std::endl;
    fout << "1" << std::endl; // num real tags.
    fout << "0.0" << std::endl; // time value.
    fout << "3" << std::endl; // num int tags.
    fout << "0" << std::endl; // the time step
    fout << "1" << std::endl; // 1-component scalar field.
    fout << m_num_elements << std::endl; // number of elements

    if (m_binary) {
        for (size_t i=0; i<m_num_elements; i++) {
            int elem_idx = i+1;
            fout.write((char*)&elem_idx, sizeof(int));
            fout.write((char*)&field[i], sizeof(Float));
        }
    } else {
        for (size_t i=0; i<m_num_elements; i++) {
            int elem_idx = i+1;
            fout << elem_idx << " " << field[i] << std::endl;
        }
    }

    fout << "$EndElementData" << std::endl;
    fout.flush();
}

void MshSaver::save_elem_vector_field(const std::string& fieldname, const VectorF& field) {
    assert(field.size() == m_num_elements * m_dim);
    fout << "$ElementData" << std::endl;
    fout << 1 << std::endl; // num string tags.
    fout << "\"" << fieldname << "\"" << std::endl;
    fout << "1" << std::endl; // num real tags.
    fout << "0.0" << std::endl; // time value.
    fout << "3" << std::endl; // num int tags.
    fout << "0" << std::endl; // the time step
    fout << "3" << std::endl; // 3-component vector field.
    fout << m_num_elements << std::endl; // number of elements

    const Float zero = 0.0;
    if (m_binary) {
        for (size_t i=0; i<m_num_elements; i++) {
            int elem_idx = i+1;
            fout.write((char*)&elem_idx, sizeof(int));
            if (m_dim == 3)
                fout.write((char*)&field[i*3], sizeof(Float) * 3);
            else if (m_dim == 2) {
                fout.write((char*)&field[i*2], sizeof(Float) * 2);
                fout.write((char*)&zero, sizeof(Float)); // set z=0
            }
        }
    } else {
        for (size_t i=0; i<m_num_elements; i++) {
            int elem_idx = i+1;
            if (m_dim == 3) {
                fout << elem_idx
                    << " " << field[i*3]
                    << " " << field[i*3+1]
                    << " " << field[i*3+2]
                    << std::endl;
            } else if (m_dim == 2) {
                fout << elem_idx
                    << " " << field[i*2]
                    << " " << field[i*2+1]
                    << " " << zero
                    << std::endl;
            }
        }
    }

    fout << "$EndElementData" << std::endl;
    fout.flush();
}

void MshSaver::save_elem_tensor_field(const std::string& fieldname, const VectorF& field) {
    assert(field.size() == m_num_elements * m_dim * (m_dim - 1));
    fout << "$ElementData" << std::endl;
    fout << 1 << std::endl; // num string tags.
    fout << "\"" << fieldname << "\"" << std::endl;
    fout << "1" << std::endl; // num real tags.
    fout << "0.0" << std::endl; // time value.
    fout << "3" << std::endl; // num int tags.
    fout << "0" << std::endl; // the time step
    fout << "9" << std::endl; // 9-component tensor field.
    fout << m_num_elements << std::endl; // number of elements

    const Float zero = 0.0;
    if (m_binary) {
        for (size_t i=0; i<m_num_elements; i++) {
            int elem_idx = i+1;
            fout.write((char*)&elem_idx, sizeof(int));
            if (m_dim == 3) {
                const VectorF& val = field.segment(i*6, 6);
                Float tensor[9] = {
                    val[0], val[5], val[4],
                    val[5], val[1], val[3],
                    val[4], val[3], val[2] };
                fout.write((char*)tensor, sizeof(Float) * 9);
            } else if (m_dim == 2) {
                const VectorF& val = field.segment(i*3, 3);
                Float tensor[9] = {
                    val[0], val[2], zero,
                    val[2], val[1], zero,
                      zero,   zero, zero };
                fout.write((char*)tensor, sizeof(Float)*9);
            }
        }
    } else {
        for (size_t i=0; i<m_num_elements; i++) {
            int elem_idx = i+1;
            if (m_dim == 3) {
                const VectorF& val = field.segment(i*6, 6);
                fout << elem_idx
                    << " " << val[0]
                    << " " << val[5]
                    << " " << val[4]
                    << " " << val[5]
                    << " " << val[1]
                    << " " << val[3]
                    << " " << val[4]
                    << " " << val[3]
                    << " " << val[2]
                    << std::endl;
            } else if (m_dim == 2) {
                const VectorF& val = field.segment(i*3, 3);
                fout << elem_idx
                    << " " << val[0]
                    << " " << val[2]
                    << " " << zero
                    << " " << val[2]
                    << " " << val[1]
                    << " " << zero
                    << " " << zero 
                    << " " << zero 
                    << " " << zero 
                    << std::endl;
            }
        }
    }

    fout << "$EndElementData" << std::endl;
    fout.flush();
}

void MshSaver::save_binary_mesh(const VectorF& nodes, const VectorI& elements,
        size_t nodes_per_element) {
    m_num_nodes = nodes.size() / m_dim;
    m_num_elements = elements.size() / nodes_per_element;

    fout << "$MeshFormat" << std::endl;
    fout << "2.2 1 " << sizeof(double) << std::endl;
    int one = 1;
    fout.write((char*)&one, sizeof(int));
    fout << "$EndMeshFormat" << std::endl;

    // Save nodes.
    fout << "$Nodes" << std::endl;
    fout << m_num_nodes << std::endl;
    for (size_t i=0; i<nodes.size(); i+=m_dim) {
        const VectorF& v = nodes.segment(i,m_dim);
        int node_idx = i/m_dim+1;
        fout.write((char*)&node_idx, sizeof(int));
        fout.write((char*)v.data(), sizeof(Float)*m_dim);

        // for 2D shapes, z coordinate is always 0.
        if (m_dim == 2) {
            const Float zero = 0.0;
            fout.write((char*)&zero, sizeof(Float));
        }
    }
    fout << "$EndNodes" << std::endl;

    // Save elements.
    fout << "$Elements" << std::endl;
    fout << m_num_elements << std::endl;

    int elem_type = 0;
    if (nodes_per_element == 3) {
        elem_type = 2;
    } else if (nodes_per_element == 4) {
        elem_type = 4;
    } else if (nodes_per_element == 8) {
        elem_type = 5;
    } else {
        assert(false);
    }

    int num_elems = m_num_elements;
    int tags = 0;
    fout.write((char*)&elem_type, sizeof(int));
    fout.write((char*)&num_elems, sizeof(int));
    fout.write((char*)&tags, sizeof(int));
    for (size_t i=0; i<elements.size(); i+=nodes_per_element) {
        int elem_num = i/nodes_per_element + 1;
        VectorI elem = elements.segment(i, nodes_per_element) +
            VectorI::Ones(nodes_per_element);
        fout.write((char*)&elem_num, sizeof(int));
        fout.write((char*)elem.data(), sizeof(int)*nodes_per_element);
    }
    fout << "$EndElements" << std::endl;
    fout.flush();
}

void MshSaver::save_ascii_mesh(const VectorF& nodes, const VectorI& elements,
        size_t nodes_per_element) {
    m_num_nodes = nodes.size() / m_dim;
    m_num_elements = elements.size() / nodes_per_element;

    fout << "$MeshFormat" << std::endl;
    fout << "2.2 0 " << sizeof(double) << std::endl;
    fout << "$EndMeshFormat" << std::endl;

    // Save nodes.
    fout << "$Nodes" << std::endl;
    fout << m_num_nodes << std::endl;
    for (size_t i=0; i<nodes.size(); i+=m_dim) {
        const VectorF& v = nodes.segment(i,m_dim);
        int node_idx = i/m_dim+1;
        fout << node_idx << " " << v[0] << " " << v[1] << " ";
        if (m_dim == 2) {
            fout << 0.0 << std::endl;
        } else {
            fout << v[2] << std::endl;
        }
    }
    fout << "$EndNodes" << std::endl;

    // Save elements.
    fout << "$Elements" << std::endl;
    fout << m_num_elements << std::endl;

    int elem_type = 0;
    if (nodes_per_element == 3) {
        elem_type = 2;
    } else if (nodes_per_element == 4) {
        elem_type = 4;
    } else if (nodes_per_element == 8) {
        elem_type = 5;
    } else {
        assert(false);
    }

    int num_elems = m_num_elements;
    int tags = 0;
    for (size_t i=0; i<elements.size(); i+=nodes_per_element) {
        int elem_num = i/nodes_per_element + 1;
        VectorI elem = elements.segment(i, nodes_per_element) +
            VectorI::Ones(nodes_per_element);

        fout << elem_num << " " << elem_type << " " << tags << " ";
        for (size_t j=0; j<nodes_per_element; j++) {
            fout << elem[j] << " ";
        }
        fout << std::endl;
    }
    fout << "$EndElements" << std::endl;

    fout.flush();
}
