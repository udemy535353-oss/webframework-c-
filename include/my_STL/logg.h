//
// Created by halim on 26.02.2026.
//

#ifndef MY_STL_LOGG_H
#define MY_STL_LOGG_H


// LOG FİLES SAVED AT

#include <filesystem>
#include <string_view>
#include <fstream>
#include <iostream>

namespace nsq {
    class LogStream {
    public:
        LogStream() = default;
        ~LogStream() = default;

        static bool logf( std::string_view content);




    };
}
#endif //MY_STL_LOGG_H