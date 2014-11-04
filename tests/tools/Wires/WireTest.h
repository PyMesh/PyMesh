#pragma once

#include <string>

#include <Core/EigenTypedef.h>
#include <Core/Exception.h>
#include <Misc/Environment.h>

#include <Wires/WireNetwork/WireNetwork.h>

class WireTest : public ::testing::Test {
    protected:
        virtual void SetUp() {
            std::string project_dir = Environment::get("PYMESH_PATH");
            m_data_dir = project_dir + "/tests/tools/Wires/data/";
        }

        WireNetwork load_wire(const std::string& filename) {
            return WireNetwork(m_data_dir + filename);
        }

        void ASSERT_VECTOR_EQ(const VectorF& v1, const VectorF& v2) {
            ASSERT_EQ(v1.size(), v2.size());
            const size_t num_entries = v1.size();
            for (size_t i=0; i<num_entries; i++) {
                ASSERT_FLOAT_EQ(v1[i], v2[i]);
            }
        }

    protected:
        std::string m_data_dir;
};
