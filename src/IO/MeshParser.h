/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once
#include <vector>
#include <string>

#include <Core/EigenTypedef.h>

namespace PyMesh {

class MeshParser {
    public:
        static MeshParser* create_parser(const std::string& filename);

    public:
        typedef std::vector<std::string> AttrNames;
        virtual ~MeshParser() {}

        /**
         * Parse input file.
         * @return true only if file is successfully parsed.
         */
        virtual bool parse(const std::string& filename)=0;

        virtual size_t num_vertices() const=0;
        virtual size_t num_faces() const=0;
        virtual size_t num_voxels() const=0;
        virtual size_t num_attributes() const=0;

        virtual AttrNames get_attribute_names() const=0;
        virtual size_t get_attribute_size(const std::string& name) const=0;

        virtual void export_vertices(Float* buffer)=0;
        virtual void export_faces(int* buffer)=0;
        virtual void export_voxels(int* buffer)=0;
        virtual void export_attribute(const std::string& name, Float* buffer)=0;

        virtual size_t dim() const {return 3;}
        virtual size_t vertex_per_voxel() const {return 0;}
        virtual size_t vertex_per_face() const  {return 3;}
};

}
