//
// Created by halim on 6.03.2026.
//

#ifndef MY_STL_FILE_SYSTEM_H
#define MY_STL_FILE_SYSTEM_H
#include <string>
#include <filesystem>
#include <windows.h>

namespace nsq {
    inline std::string get_exe_directory() {
        char buffer[MAX_PATH];

        GetModuleFileNameA(NULL, buffer, MAX_PATH);

        std::filesystem::path exe_path(buffer);



        return exe_path.parent_path().string() + "\\";
    }
}
#endif //MY_STL_FILE_SYSTEM_H