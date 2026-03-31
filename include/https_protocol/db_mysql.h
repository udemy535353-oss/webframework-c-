//
// Created by halim on 6.03.2026.
//

#ifndef MY_STL_DB_MYSQL_H
#define MY_STL_DB_MYSQL_H


#include <mysql.h>
#include <string>

#include <mutex>
#include <vector>








namespace nsq {
    class maria_db {
        public:
            maria_db() {
                conn = mysql_init(NULL);
                if (conn == NULL) {
                    std::cerr << "MySQL init hatasi!" << std::endl;
                }
            }

        bool execute(const std::string &query) {

                std::lock_guard<std::mutex> lock(db_mutex);


                return mysql_query(conn, query.c_str()) == 0;
            }


        std::vector<std::map<std::string, std::string>> fetch_all(const std::string& query) {
                std::lock_guard<std::mutex> lock(db_mutex);

                std::vector<std::map<std::string, std::string>> results;
                if (mysql_query(conn, query.c_str())) return results;

                MYSQL_RES* res = mysql_store_result(conn);
                if (!res) return results;

                MYSQL_ROW row;
                unsigned int num_fields = mysql_num_fields(res);
                MYSQL_FIELD* fields = mysql_fetch_fields(res);

                while ((row = mysql_fetch_row(res))) {
                    std::map<std::string, std::string> current_row;
                    for (unsigned int i = 0; i < num_fields; i++) {

                        current_row[fields[i].name] = (row[i] ? row[i] : "NULL");
                    }
                    results.push_back(current_row);
                }

                mysql_free_result(res);
                return results;
            }


        std::map<std::string, std::string> fetch_one(const std::string& query) {
                std::lock_guard<std::mutex> lock(db_mutex);

                std::map<std::string, std::string> result;
                if (mysql_query(conn, query.c_str())) return result;

                MYSQL_RES* res = mysql_store_result(conn);
                if (!res) return result;

                MYSQL_ROW row = mysql_fetch_row(res);
                if (row) {
                    unsigned int num_fields = mysql_num_fields(res);
                    MYSQL_FIELD* fields = mysql_fetch_fields(res);

                    for (unsigned int i = 0; i < num_fields; i++) {

                        result[fields[i].name] = (row[i] ? row[i] : "NULL");
                    }
                }

                mysql_free_result(res);
                return result;
            }

        bool connect(const char* host, const char* user, const char* pass, const char* db) {

                if (mysql_real_connect(conn, host, user, pass, db, 3306, NULL, 0)) {
                    std::cout << "[L11] MariaDB Baglantisi Basarili!" << std::endl;
                    return true;
                }
                return false;
            }


        bool commit_connection() {
                return mysql_query(conn, "COMMIT;") == 0;
            }

        ~maria_db() {
                if (conn) mysql_close(conn);
            }

        void close() {
                if (conn != NULL) {
                    mysql_close(conn);
                    conn = NULL;
                    std::cout << "[L11] MariaDB Baglantisi Guvenli Bir Sekilde Kapatildi." << std::endl;
                }
            }
    private:
        MYSQL* conn;
        std::mutex db_mutex;
    };
}


#endif //MY_STL_DB_MYSQL_H