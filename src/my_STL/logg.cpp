//
// Created by halim on 26.02.2026.
//

#include <my_STL/logg.h>
#include <iomanip>
#include <ctime>

namespace nsq {




    bool LogStream::logf(std::string_view content) {

        std::string_view path =  "../logs/logs.txt";



        std::filesystem::path p(path);
        if (p.has_parent_path()) {
            std::filesystem::create_directories(p.parent_path());
        }

        auto now = std::time(nullptr);
        auto tm_info = std::localtime(&now);


        std::ostringstream time_ss;
        time_ss << std::put_time(tm_info, "[%Y-%m-%d %H:%M:%S] ");

        std::ofstream Lfile(p,std::ios::binary | std::ios::out | std::ios::app);

        if (!Lfile.is_open()) {
            std::cerr<<"has file error occured path ->  "<<path<<std::endl;
            return false;
        }

        Lfile<<time_ss.str()<<"  "<<content<<"\n";

        return Lfile.good();

    }
}