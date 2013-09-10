#pragma once

#include "MeshParser.h"
#include <vector>
#include <list>
#include <string>
#include <tr1/memory>

#include "EigenTypedef.h"

namespace Zhou {
class MshLoader;
}

class MSHParser : public MeshParser {
    public:
        typedef MeshParser::AttrNames AttrNames;
        virtual ~MSHParser() {}

        virtual bool parse(const std::string& filename);

        virtual size_t num_vertices() const;
        virtual size_t num_faces() const;
        virtual size_t num_voxels() const;
        virtual size_t num_attributes() const;

        virtual AttrNames get_attribute_names() const;
        virtual size_t get_attribute_size(const std::string& name) const;

        virtual void export_vertices(Float* buffer);
        virtual void export_faces(int* buffer);
        virtual void export_voxels(int* buffer);
        virtual void export_attribute(const std::string& name, Float* buffer);

        virtual size_t vertex_per_voxel() const;
        virtual size_t vertex_per_face() const;

    protected:
        void extract_faces_and_voxels();
        void extract_surface_from_volume();
        const VectorF& get_attribute(const std::string& name) const;

        typedef std::tr1::shared_ptr<Zhou::MshLoader> LoaderPtr;
        LoaderPtr m_loader;

        VectorI m_faces;
        VectorI m_voxels;
};
