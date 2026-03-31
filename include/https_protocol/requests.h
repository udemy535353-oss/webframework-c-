#ifndef MY_STL_REQUESTS_H
#define MY_STL_REQUESTS_H
#include <string>
#include <map>
#include <sstream>
#include <vector>

namespace nsq {
    inline std::string url_decode(const std::string& text) {
        std::string out;
        for (size_t i = 0; i < text.length(); ++i) {
            if (text[i] == '%' && i + 2 < text.length()) {
                out += (char)std::stoi(text.substr(i + 1, 2), nullptr, 16);
                i += 2;
            } else if (text[i] == '+') out += ' ';
            else out += text[i];
        }
        return out;
    }

    struct Request {
        std::string method, path, body, client_ip;
        int client_port;
        std::map<std::string, std::string> headers;
        std::map<std::string, std::string> query;

        void parse_query(const std::string& full_path) {
            size_t q = full_path.find('?');
            if (q == std::string::npos) { path = full_path; return; }
            path = full_path.substr(0, q);
            std::stringstream ss(full_path.substr(q + 1));
            std::string pair;
            while (std::getline(ss, pair, '&')) {
                size_t eq = pair.find('=');
                if (eq != std::string::npos)
                    query[url_decode(pair.substr(0, eq))] = url_decode(pair.substr(eq + 1));
            }
        }

        std::map<std::string, std::string> parse_form() const {
            std::map<std::string, std::string> form;
            std::stringstream ss(body);
            std::string pair;
            while (std::getline(ss, pair, '&')) {
                size_t eq = pair.find('=');
                if (eq != std::string::npos)
                    form[url_decode(pair.substr(0, eq))] = url_decode(pair.substr(eq + 1));
            }
            return form;
        }
    };
}
#endif