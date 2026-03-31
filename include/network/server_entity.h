//
// Created by halim on 11.03.2026.
//

#ifndef MY_STL_SERVER_ENTITY_H
#define MY_STL_SERVER_ENTITY_H


#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
namespace nsq {
    bool inline fire() {

        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            return 1;
        }


        SOCKET serv = socket(AF_INET, SOCK_STREAM, 0);
        if (serv == INVALID_SOCKET) {
            WSACleanup();
            return 1;
        }

        sockaddr_in  addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(3153);
        addr.sin_addr.s_addr = INADDR_ANY;

        inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);

        std::cout << "[L11] Sunucuya kapiyi calmaya gidiliyor..." << std::endl;
        if (connect(serv, (struct sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
            std::cout << "[!] Baglanti Reddedildi! Kod: " << WSAGetLastError() << std::endl;
            closesocket(serv);
            WSACleanup();
            return 1;
        }


        std::string mesaj = "hüseyin hüseyin hüseyin!";
        send(serv, mesaj.c_str(), (int)mesaj.length(), 0);

        std::cout << "[+] Veri hedefe ulasti. Gorev tamam." << std::endl;


        closesocket(serv);
        WSACleanup();

        return 0;

    }
}
#endif //MY_STL_SERVER_ENTITY_H