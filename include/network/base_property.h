//
// Created by halim on 11.03.2026.
//

#ifndef MY_STL_BASE_PROPERTY_H
#define MY_STL_BASE_PROPERTY_H


#include <winsock2.h>
#include <my_STL/logg.h>
#include <iostream>
#include <ws2tcpip.h>
#pragma comment(lib,"ws2_32.lib")
namespace nsq {
    class TopSocket {

    public:


        TopSocket(int p) : port(p), sock(INVALID_SOCKET) {

            startup_handler();
            std::cout<<"[LOG] TopSocket ->> "<<port<<std::endl;

            sock = socket(AF_INET,SOCK_STREAM,0);

            u_long mode = 1;

            ioctlsocket(sock, FIONBIO, &mode);



        }


        bool inline setup() {








            addr.sin_family = AF_INET;
            addr.sin_port = htons(port);
            addr.sin_addr.s_addr = INADDR_ANY;


            if (bind(sock,reinterpret_cast<sockaddr *>(&addr),sizeof(addr)) == SOCKET_ERROR) {

                int err = WSAGetLastError();
                log_center.logf("error at bind -> start at line 50");
                std::cout<<"[LOG] Error code in winsock ->> "<<err<<" "<<__LINE__<<std::endl;

                closesocket(sock);

                WSACleanup();

                return false;
            };
            return true;
        }

        bool listen_somaxconn() {
            if ((listen(sock,SOMAXCONN)) == SOCKET_ERROR) {

                int error = WSAGetLastError();

                std::cout<<"[LOG] server closed with error at" <<__LINE__<<"error is ->"<<error<<std::endl;
                WSACleanup();
                closesocket(sock);

                return false;

            }

            std::cout<<"listening socket ->> "<<port<<std::endl;

            return true;
        }

        SOCKET accept_client() {
            sockaddr_in client_addr;
            socklen_t client_addr_len = sizeof(client_addr);
            SOCKET client = accept(sock,reinterpret_cast<sockaddr *>(&client_addr),&client_addr_len);
            return client;
        }

        bool send_message(SOCKET client,const std::string & message) {
            int result = send(client,message.c_str(),message.length(),0);
            return result != SOCKET_ERROR ? true : false;
        }




        std::string receive_message(SOCKET client) {
            char buffer[4096];
            int bytes_read = recv(client,buffer,sizeof(buffer)-1,0);

            if (bytes_read > 0) {
                buffer[bytes_read] = '\0';
                return std::string(buffer);
            }

            return "";
        }

        ~TopSocket() {
            if (sock != INVALID_SOCKET) closesocket(sock);
        }


    private:
        SOCKET sock;

        int port;
        sockaddr_in addr;
        nsq::LogStream log_center;

        void inline startup_handler() {



            WSADATA wsa;
            if (WSAStartup(MAKEWORD(2,2), &wsa)) {
                exit(EXIT_FAILURE);
            }
        }
    };
}

#endif //MY_STL_BASE_PROPERTY_H