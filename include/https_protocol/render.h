#ifndef MY_STL_RENDER_H
#define MY_STL_RENDER_H
#include <fstream>
#include <map>
#include <string>
#include <https_protocol/file_system.h>

namespace nsq {
    class TemplateEngine {
    public:
        static std::string render(const std::string& path, const std::map<std::string, std::string> &values = {}) {
            std::string full_path = get_exe_directory() + "templates\\" + path;
            std::ifstream file(full_path, std::ios::binary | std::ios::ate);
            if (!file.is_open()) return "<h1>Template Bulunamadi!</h1>";

            std::streamsize size = file.tellg();
            file.seekg(0, std::ios::beg);
            std::string buffer(size, '\0');
            file.read(&buffer[0], size);

            for (auto const& [key, val] : values) {
                std::string placeholder = "{{" + key + "}}";
                size_t pos = 0;
                while ((pos = buffer.find(placeholder, pos)) != std::string::npos) {
                    buffer.replace(pos, placeholder.length(), val);
                    pos += val.length();
                }
            }
            return buffer;
        }
    };
}
#endif