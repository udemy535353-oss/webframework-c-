//
// Created by halim on 25.02.2026.
//


#include <memory/mm_arena.h>
#include <my_STL/logg.h>
#include <memory/mm_vector.h>
#include <database/db_main.h>
#include <https_protocol/requests.h>
#include <https_protocol/framework.h>
#include <https_protocol/start_server.h>
#include <database/db_io.h>
#include <https_protocol/db_mysql.h>
#include <https_protocol/multi_tasking.h>
#include <https_protocol/render.h>
#include <winsock2.h>
#include <string>
#include <ws2tcpip.h>
#include <https_protocol/file_system.h>
#include <network/base_property.h>
#include <network/server_entity.h>
#pragma comment(lib, "ws2_32.lib")

/*
struct User {
    char name[32];
    int age;
    char story[255];
    char st[255];
};
#pragma pack(push, 1)
struct Web {
    char name[32];
    float Prank;
};
#pragma pack(pop)
using namespace std;
#include <iostream>
class Entity {
public:
    std::string name;
    int power;

    Entity(std::string n, int p) : name(std::move(n)), power(p) {
        std::cout<<"start from memory"<< std::endl;

    }

    ~Entity() {
        std::cout<<"killed"<<name<<std::endl;
    }
};
*/



int main() {








    nsq::TopSocket server(3153);


    if (!server.setup()) {
        return 1; // Bind patlarsa çık
    }

    if (!server.listen_somaxconn()) {
        return 1; // Listen patlarsa çık
    }

    // 2. ANA DÖNGÜ: Dükkan her zaman açık!


    nsq::fire();


    while (true) {


        // Müşteri gelene kadar kod burada durur (Blocking)
        SOCKET client = server.accept_client();

        if (client != INVALID_SOCKET) {
            std::cout << "[+] Baglanti kabul edildi!" << std::endl;

            // 3. Veri Alışverişi
            std::string msg = server.receive_message(client);
            if (!msg.empty()) {
                std::cout << "Gelen: " << msg << std::endl;
                server.send_message(client, "Mermi gibi aldim mesajini!");
            }

            // 4. Müşteriyi Uğurla ama Server'ı Kapatma!
            closesocket(client);
        }
    }



    /*std::cout<<nsq::get_exe_directory()<<std::endl;
    nsq::memory mm(1024*1024);

    auto hero = mm.arena_new<Entity>("fdf",999);

    nsq::LogStream::logf("C:\\test\\error.txt","Memory has created succes...");"""

    nsq::vector<int> sayilar;
    sayilar.push_back(1);
    sayilar.push_back(2);
    sayilar.push_back(3);

    cout<<sayilar[0];

    nsq::maria_db db;




    NeoFramework app;





    if (!db.connect("localhost", "halim", "Br66p$MYR@Hs8YftEL@VxnHk2Grk^ZWH&P44QieDRWdLM3VVu", "users")) {
        std::cerr << "Veritabanina baglanilamadi!" << std::endl;
        return -1;
    }









    app.get("/", [](const nsq::Request& req) {
        return nsq::TemplateEngine::render("file.html", {
    {"nvlink", "activation label replaced with NeoFramework"},

});
    });

    app.get("/halim", [](const nsq::Request& req) {
        return "<h2>Selam Halim!</h2><p>Server canavar gibi calisiyor.</h2>";
    });

    app.get("/login", [](const nsq::Request& req) {
        return nsq::TemplateEngine::render("login.html");
    });

    app.post("/submit", [&db](const nsq::Request& req) {
        auto form = req.parse_form();

        std::string user = form["username"];

        std::string sql = "INSERT INTO users (name,email,username,password,date,followers,profile,provider) VALUES ('" + user + "', '" + "babam@gmail.com" + "', '" + user + "', '" + form["password"] + "', '" + "2025" + "', '" + "1" + "', '" + "annen.jpg" + "', '" + "google" + "')";
        db.execute(sql);

        db.commit_connection();


        return "<h1>Sistem mermi gibi çalışıyor bu arada hoş geld,n "+user +"</h1>";
    });




    app.get("/profile", [&db](const nsq::Request& req) {

        auto user = db.fetch_one("select * from users");


        std::cout<<req.client_ip<<std::endl;


        return nsq::TemplateEngine::render("file.html",{
            {"name",user["name"]},
            {"id",user["ıd"]},
            {"email",user["email"]},
            {"username",user["username"]}



            });



    });



    std::cout << "[L11] Server Thread Pool ile baslatildi..." << std::endl;

    app.run(8080,16);


















        Web found = db.search_from_feature([](const Web&u) {
            return std::string(u.name)=="gogolo.com" and u.Prank > 5;

        });

        if  (std::string(found.name) == "gogolo.com") {

            std::cout<<found.name<<std::endl;

        }
        else {


            std::cout<<"bulamadık"<<std::endl;
        }*/








    //User uss = db.select_from_indices;

    //std::cout<<uss.name<<std::endl;
}