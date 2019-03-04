/* This file is part of PyMesh. Copyright (c) 2019 by Qingnan Zhou */
#pragma once

#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
#include <chrono>

class Timer {
    public:
        using TimePoint = std::chrono::time_point<
            std::chrono::high_resolution_clock>;
        Timer(const std::string& name) : name(name) {
            auto t = std::chrono::high_resolution_clock::now();
            names.push_back("start");
            tiks.push_back(t);
        }

        void tik(const std::string& name, bool print=false) {
            auto t = std::chrono::high_resolution_clock::now();
            names.push_back(name);
            tiks.push_back(t);
            if (print) {
                size_t num_tiks = tiks.size();
                std::chrono::duration<double> interval =
                    tiks[num_tiks-1] - tiks[num_tiks-2];
                std::cout << name << " : " << interval.count() << " s." << std::endl;
            }
        }

        void summary() {
            double total_duration = 0.0;
            size_t num_tiks = tiks.size();
            std::cout << std::setfill('=') << std::setw(50) << "="
                << std::setfill(' ')<< std::endl;
            std::cout << "|" << name << " summary: " << std::endl;
            for (size_t i=1; i<num_tiks; i++) {
                std::chrono::duration<double> interval =
                    tiks[i] - tiks[i-1];
                std::cout << "|" << std::setw(20) << names[i] << " : "
                    << interval.count() << " s." << std::endl;
                total_duration += interval.count();
            }
            std::cout << std::setfill('=') << std::setw(50) << "="
                << std::setfill(' ')<< std::endl;
            std::cout << "total: " << total_duration << " s." << std::endl;
        }

    private:
        std::string name;
        std::vector<std::string> names;
        std::vector<TimePoint> tiks;
};
