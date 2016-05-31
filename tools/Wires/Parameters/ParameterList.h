/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <cassert>
#include <list>
#include <Core/EigenTypedef.h>

#include "PatternParameter.h"

namespace PyMesh {

class ParameterList {
    public:
        typedef ParameterCommon::TargetType TargetType;
        typedef PatternParameter::Variables Variables;

    public:
        virtual ~ParameterList() {}

    public:
        // These functions are used to facilitate the parameter sweep.
        size_t get_num_dofs() const { return m_params.size(); }

        VectorF get_dofs() const {
            const size_t num_params = m_params.size();
            VectorF dofs(num_params);
            size_t count = 0;
            for (auto param : m_params) {
                dofs[count] = param->get_value();
                count++;
            }
            return dofs;
        }

        void set_dofs(const VectorF& dofs) {
            assert(size_t(dofs.size()) == m_params.size());
            size_t count = 0;
            for (auto param : m_params) {
                param->set_value(dofs[count]);
                count++;
            }
        }

    public:
        typedef std::list<PatternParameter::Ptr>::iterator iterator;
        typedef std::list<PatternParameter::Ptr>::const_iterator const_iterator;

        iterator begin() { return m_params.begin(); }
        const_iterator begin() const { return m_params.begin(); }
        iterator end() { return m_params.end(); }
        const_iterator end() const { return m_params.end(); }

    protected:
        std::list<PatternParameter::Ptr> m_params;
};

}
