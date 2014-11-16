#pragma once

#include <cassert>
#include <list>
#include <Core/EigenTypedef.h>

#include "PatternParameter.h"

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
            assert(dofs.size() == m_params.size());
            size_t count = 0;
            for (auto param : m_params) {
                param->set_value(dofs[count]);
                count++;
            }
        }

    protected:
        std::list<PatternParameter::Ptr> m_params;
};
