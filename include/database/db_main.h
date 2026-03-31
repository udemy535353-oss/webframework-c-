#ifndef MY_STL_DB_MAIN_H
#define MY_STL_DB_MAIN_H

#include <iostream>
#include <utility>
#include <fstream>
#include <memory>
#include <cstring>
#include <filesystem>
#include <my_STL/logg.h>



// nsq Micro(plus) database
namespace nsq {
    template<typename T>
    class db {
    private:
        char* m_start;
        char* m_current;
        char* m_end;


        static constexpr const char* BASE_DIR = "../data/";

        size_t pageSize;

        const char* m_filename;


        std::string m_fullPath;

        static constexpr size_t FILE_SIZE = 4096;

        std::fstream m_file;

        nsq::LogStream log;



    public:
        db(const char* filename = "database.bin") {


            m_fullPath = std::string(BASE_DIR) + filename;
            m_filename = m_fullPath.c_str();

            m_start = new char[FILE_SIZE];
            m_end = m_start + FILE_SIZE;
            m_current = m_start;

            // Klasörü oluştur
            std::filesystem::path p(m_fullPath);
            if (p.has_parent_path()) std::filesystem::create_directories(p.parent_path());

            m_file.open(m_fullPath, std::ios::in | std::ios::out | std::ios::binary);

            if (!m_file.is_open()) {
                std::ofstream create(m_fullPath, std::ios::binary);
                size_t mühür = sizeof(T);
                create.write(reinterpret_cast<const char*>(&mühür), sizeof(size_t));
                create.close();
                m_file.open(m_fullPath, std::ios::in | std::ios::out | std::ios::binary);
            } else {
                size_t d_mühür = 0;
                m_file.seekg(0, std::ios::beg);
                m_file.read(reinterpret_cast<char*>(&d_mühür), sizeof(size_t));
                if (d_mühür != sizeof(T)) {
                    std::cerr << "HATA: dosya uyuşmazlığı" << std::endl;
                    exit(EXIT_FAILURE);
                }
            }

            // TEK VE GERÇEK OKUMA BURASI:
            m_file.clear();
            m_file.seekg(sizeof(size_t), std::ios::beg); // Mührü (kalbi) diskte bırak, RAM'e alma!
            m_file.read(m_start, FILE_SIZE - sizeof(size_t));
            m_current = m_start + m_file.gcount();
        }

        bool insert(const T& data) {

            if (m_current + sizeof(T) > m_end) return false;

            // 1. RAM'deki mevcut doluluğu hesapla (Byte cinsinden)
            std::streamsize offset_in_ram = m_current - m_start;

            // 2. Diskteki yazma pozisyonu: Mühür + RAM'deki ofset
            std::streamsize write_pos = static_cast<std::streamsize>(sizeof(size_t)) + offset_in_ram;

            // 3. RAM'e kopyala
            std::memcpy(m_current, &data, sizeof(T));

            // 4. Dosyaya tam o noktaya yaz
            if (m_file.is_open()) {
                m_file.clear();
                m_file.seekp(write_pos, std::ios::beg);
                m_file.write(reinterpret_cast<const char*>(&data), sizeof(T));
                m_file.flush();
            }

            // 5. Pointer'ı bir sonraki kayıt için kaydır
            m_current += sizeof(T);
            return true;
        }
        size_t get_file_size() {
            std::filesystem::path p(m_fullPath);
            if (std::filesystem::exists(p)) {
                return std::filesystem::file_size(p); // Byte cinsinden döner
            }
            return 0;
        }

        ~db() {
            delete[] m_start;

            m_file.close();
        }
        T select_from_indices(int index) {
            if (index < 0) return T{};
            T temp;

            if (m_file.is_open()) {

                m_file.clear();

                auto pos = static_cast<std::streamsize>(sizeof(size_t)) +
                (static_cast<std::streamsize>(index) * static_cast<std::streamsize>(sizeof(T)));

                m_file.seekg(0, std::ios::end);
                if (pos + sizeof(T) > m_file.tellg()) {
                    log.logf("HATA: Olmayan veriye erisim! Index: ");
                    return T{};
                }

                m_file.seekg(pos,std::ios::beg);



                m_file.read(reinterpret_cast<char*>(&temp),sizeof(T));








            }

            return temp;
        }

        // O(n^2) search

        //search from Record argument

        template<typename Predictor>

        T search_from_feature(Predictor condition) {


                T temp={};

                if (m_file.is_open()) {

                    m_file.clear();

                    m_file.seekg(sizeof(size_t), std::ios::beg);

                    while (m_file.read(reinterpret_cast<char*>(&temp),sizeof(T))) {
                        if (condition(temp)) {

                            return temp;
                        }
                    }








                }

                return T{};


            }

        template<typename...Args>
        bool multi_insertion(Args&&...args) {

            static_assert((std::is_convertible_v<Args, T> && ...),
                      "HATA: All types same of ..db<TYPE!>");


            bool result = (... && insert(std::forward<Args>(args)));

            if (result) {
                log.logf("multi process succse.");
            } else {
                log.logf("ERROR: multi_process");
            }

            return result;
        }










    };
    };

#endif