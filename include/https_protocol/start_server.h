#ifndef MY_STL_LAUNCH_SEVER_H
#define MY_STL_LAUNCH_SEVER_H

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>

#pragma comment(lib, "ws2_32.lib")

namespace nsq {

    inline void NeoWinshock() {
        WSADATA wsa;
        if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
            std::cerr << "WinSock baslatilamadi!" << std::endl;
            exit(1);
        }
    }


    inline SOCKET Neo(int port) {
        SOCKET server_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (server_fd == INVALID_SOCKET) {
            std::cerr << "Soket hatasi: " << WSAGetLastError() << std::endl;
            return INVALID_SOCKET;
        }

        int opt = 1;
        setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));

        sockaddr_in adres{};
        adres.sin_family = AF_INET;
        adres.sin_addr.s_addr = INADDR_ANY;
        adres.sin_port = htons(port);

        if (bind(server_fd, (struct sockaddr*)&adres, sizeof(adres)) == SOCKET_ERROR) {
            std::cerr << "Bind hatasi: " << WSAGetLastError() << std::endl;
            return INVALID_SOCKET;
        }

        listen(server_fd, SOMAXCONN);
        std::cout << "[L11] Server " << port << " portunda dinlemede..." << std::endl;
        return server_fd;
    }


}

#endif