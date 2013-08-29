#include "OBJMesh.h"

#include <fstream>
#include <list>

#include "IOUtils.h"

using namespace IOUtils;

void OBJMesh::read(const std::string& filename) {
    std::list<Vector3F> vertex_list;
    std::list<Vector3F> vertex_normal_list;
    std::list<Vector2F> vertex_texture_list;
    std::list<size_t> face_list;
    std::list<size_t> voxel_list;

    std::ifstream fin(filename.c_str());
    while (!fin.eof()) {
        char line_header = fin.get();
        char next_char = ' ';
        switch (line_header) {
            case 'v':
                next_char = fin.peek();
                switch (next_char) {
                    case ' ':
                    case '\t':
                        {
                            Vector3F v = parse_vertex_position(fin);
                            vertex_list.push_back(v);
                        }
                        break;
                    case 'n':
                        {
                            fin.get();
                            Vector3F vn = parse_vertex_normal(fin);
                            vertex_normal_list.push_back(vn);
                        }
                        break;
                    case 't':
                        {
                            fin.get();
                            Vector2F vt = parse_vertex_texture(fin);
                            vertex_texture_list.push_back(vt);
                        }
                        break;
                    default:
                        break;
                }
                break;
            case 'f':
                break;
            case '#':
                break;
        }
        ignore_line(fin);
    }
    fin.close();
}

Vector3F OBJMesh::parse_vertex_position(std::ifstream& fin) {
    Vector3F v;
    fin >> v[0] >> v[1] >> v[2];
    return v;
}

Vector3F OBJMesh::parse_vertex_normal(std::ifstream& fin) {
    Vector3F vn;
    fin >> vn[0] >> vn[1] >> vn[2];
    return vn;
}

Vector2F OBJMesh::parse_vertex_texture(std::ifstream& fin) {
    Vector2F vt;
    fin >> vt[0] >> vt[1];
    return vt;
}

VectorF OBJMesh::parse_face(std::ifstream& fin) {
    std::list<size_t> face;
    while (!fin.eof()) {
        size_t idx;
        fin >> idx;
        face.push_back(idx);
        char next_char = fin.peek();
        if (next_char != '/') continue;
    }
}

void OBJMesh::write(const std::string& filename) {
}
