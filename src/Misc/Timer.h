/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once

#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include "timing.h"

class Timer {
    public:
        Timer(const std::string& name) : name(name) {
            names.push_back("start");
            tiks.push_back(get_time_stamp());
        }
        
        void tik(const std::string& name, bool print=false) {
            names.push_back(name);
            tiks.push_back(get_time_stamp());
            if (print) {
                size_t num_tiks = tiks.size();
                double interval = timestamp_diff_in_seconds(
                        tiks[num_tiks-2], tiks[num_tiks-1]);
                std::cout << name << " : " << interval << " s." << std::endl;
            }
        }

        void summary() {
            double total_duration = 0.0;
            size_t num_tiks = tiks.size();
            std::cout << std::setfill('=') << std::setw(50) << "=" << std::setfill(' ')<< std::endl;
            std::cout << "|" << name << " summary: " << std::endl;
            for (size_t i=1; i<num_tiks; i++) {
                double interval = timestamp_diff_in_seconds(tiks[i-1], tiks[i]);
                std::cout << "|" << std::setw(20) << names[i] << " : " << interval << " s." << std::endl;
                total_duration += interval;
            }
            std::cout << std::setfill('=') << std::setw(50) << "=" << std::setfill(' ')<< std::endl;
            std::cout << "total: " << total_duration << " s." << std::endl;
        }

    private:
        timestamp_type get_time_stamp() {
            timestamp_type t;
            get_timestamp(&t);
            return t;
        }

    private:
        std::string name;
        std::vector<std::string> names;
        std::vector<timestamp_type> tiks;
};
