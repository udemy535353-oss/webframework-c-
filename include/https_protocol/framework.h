#ifndef MY_STL_FRAMEWORK_H
#define MY_STL_FRAMEWORK_H
#include <functional>
#include <map>
#include <mutex>
#include <string>
#include <iostream>
#include <shared_mutex>
#include <vector>
#include "requests.h"
#include "render.h"
#include "start_server.h"
#include "multi_tasking.h"
namespace nsq {
    class NeoFramework {
    public:
        using Handler = std::function<std::string(const nsq::Request&)>;

        void get(const std::string &path, Handler h) {
            std::unique_lock lock(route_mutex);
            routes["GET"][path] = h;
        }

        void post(const std::string &path, Handler h) {
            std::unique_lock lock(route_mutex);
            routes["POST"][path] = h;
        }

        void run(int port, int thread_count = 8) {
            nsq::NeoWinshock();
            SOCKET server = nsq::Neo(port);
            nsq::ThreadPool pool(thread_count);

            while (true) {
                sockaddr_in clientAddr;
                int clientAddrSize = sizeof(clientAddr);
                SOCKET client = accept(server, (struct sockaddr*)&clientAddr, &clientAddrSize);
                if (client != INVALID_SOCKET) {
                    pool.enqueue([this, client, clientAddr]() {
                        this->handle_client(client, clientAddr);
                    });
                }
            }
        }

    private:
        void handle_client(SOCKET client, sockaddr_in addr) {
            std::vector<char> buffer(8192); // Biraz nefes alsın server
            std::string raw_data;
            int bytes = recv(client, buffer.data(), (int)buffer.size(), 0);
            if (bytes <= 0) { closesocket(client); return; }
            raw_data.append(buffer.data(), bytes);

            nsq::Request req = parse_http(raw_data);

            // POST Body için Content-Length kontrolü (Eski bozuk yeri düzelttim)
            size_t cl_pos = raw_data.find("Content-Length: ");
            if (cl_pos != std::string::npos) {
                size_t header_end = raw_data.find("\r\n\r\n");
                int content_length = std::stoi(raw_data.substr(cl_pos + 16, raw_data.find("\r\n", cl_pos) - (cl_pos + 16)));
                size_t current_body_len = raw_data.length() - (header_end + 4);

                while (current_body_len < (size_t)content_length) {
                    bytes = recv(client, buffer.data(), (int)buffer.size(), 0);
                    if (bytes <= 0) break;
                    raw_data.append(buffer.data(), bytes);
                    current_body_len += bytes;
                }
                req.body = raw_data.substr(header_end + 4);
            }

            char ipStr[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &(addr.sin_addr), ipStr, INET_ADDRSTRLEN);
            req.client_ip = ipStr;

            std::string response_body = handle_request(req);
            std::string res = "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=UTF-8\r\n";
            res += "Content-Length: " + std::to_string(response_body.length()) + "\r\nConnection: close\r\n\r\n" + response_body;

            send(client, res.c_str(), (int)res.length(), 0);
            closesocket(client);
        }

        nsq::Request parse_http(const std::string& data) {
            nsq::Request req;
            size_t f_s = data.find(' '), s_s = data.find(' ', f_s + 1);
            if (f_s == std::string::npos || s_s == std::string::npos) return req;
            req.method = data.substr(0, f_s);
            std::string full_path = data.substr(f_s + 1, s_s - f_s - 1);
            req.parse_query(full_path); // Query string'i düzgün ayıkla
            return req;
        }

        std::string handle_request(const nsq::Request& req) {
            std::shared_lock lock(route_mutex);
            if (routes.count(req.method) && routes.at(req.method).count(req.path)) {
                return routes.at(req.method).at(req.path)(req);
            }
            return "<h1>404 - Mekan Sahibi Yok</h1>";
        }

        std::map<std::string, std::map<std::string, Handler>> routes;
        std::shared_mutex route_mutex;
    };
}
#endif