//
// Created by halim on 2.03.2026.
//

#ifndef MY_STL_DB_IO_H
#define MY_STL_DB_IO_H


#include <iostream>
#include <utility>
#include <fstream>
#include <memory>
#include <cstring>
#include <filesystem>
#include <variant>
#include <vector>
#include <optional>
#include <my_STL/logg.h>

#define START_OFFSET 64

// nsq Micro(plus) database
namespace nsq {
    template<typename T>
    class dbio {
    private:

        static_assert(std::is_trivially_copyable_v<T>, "HATA: T tipi karmaşık (string/vector vb.) olamaz! Sadece sabit boyutlu tipler.");









        std::string m_fullPath;



        std::fstream m_file;

        size_t m_writePtrCached;

        nsq::LogStream log;

        void update_header(size_t offset, size_t value) {
            m_file.clear();

            m_file.seekp(static_cast<std::streamsize>(offset), std::ios::beg);

            m_file.write(reinterpret_cast<const char*>(&value), sizeof(size_t));

        }

        void sync_ptr_to_disk() {
            m_file.clear();
            m_file.seekp(8, std::ios::beg);
            m_file.write(reinterpret_cast<const char*>(&m_writePtrCached), sizeof(size_t));
            m_file.flush(); // OS'e "Hemen yaz!" emri
        }



    public:
        dbio(const char* filename = "database.bin") {

            m_fullPath = "../data/data/" + std::string(filename);
            std::filesystem::create_directories("../data/data/");

            if (!std::filesystem::exists(m_fullPath)) {
                std::ofstream init(m_fullPath, std::ios::binary);
                size_t sig = sizeof(T);
                size_t start_ptr = 64;
                size_t version = 1;
                init.write(reinterpret_cast<const char*>(&sig), sizeof(size_t));       // 0-7
                init.write(reinterpret_cast<const char*>(&start_ptr), sizeof(size_t)); // 8-15
                init.write(reinterpret_cast<const char*>(&version), sizeof(size_t));   // 16-23


                std::vector<char> padding(64 - 24, 0);
                init.write(padding.data(), static_cast<std::streamsize>(padding.size()));
                init.close();




            }

            m_file.open(m_fullPath, std::ios::in | std::ios::out | std::ios::binary);

            // MÜHÜR KONTROLÜ
            m_file.seekg(8, std::ios::beg);
            m_file.read(reinterpret_cast<char*>(&m_writePtrCached), sizeof(size_t));
            size_t disk_sig = 0;
            m_file.seekg(0, std::ios::beg);
            m_file.read(reinterpret_cast<char*>(&disk_sig), sizeof(size_t));
            if (disk_sig != sizeof(T)) throw std::runtime_error("L11 Error: Signature Mismatch!");
        }


        size_t get_used_bytes() const noexcept {
            return m_writePtrCached;
        }

        bool insert(const T& data) {
            try {

                m_file.clear();




                m_file.seekp(static_cast<std::streamsize>(m_writePtrCached), std::ios::beg);

                m_file.write(reinterpret_cast<const char*>(&data), sizeof(T));



                m_writePtrCached += sizeof(T);


                return true;
            }
            catch (std::exception& e) {
                std::cerr << e.what() << std::endl;


                return false;
            }


        }

        std::optional<T> select_from_index(size_t index) {

            size_t target_pos = START_OFFSET + index * sizeof(T);


            if (target_pos >= m_writePtrCached) {
                return std::nullopt;
            }

            T record;

            m_file.clear();

            m_file.seekg(static_cast<std::streamsize>(target_pos), std::ios::beg);
            m_file.read(reinterpret_cast<char*>(&record), sizeof(T));
            return record;


        }

        template<typename Predictor>
        std::optional<T> search_from_features(Predictor condition) {
            T temp = {};



            size_t current_pos = START_OFFSET;

            m_file.clear();





            while (current_pos < m_writePtrCached) {
                m_file.seekg(static_cast<std::streamsize>(current_pos), std::ios::beg);
                if (m_file.read(reinterpret_cast<char*>(&temp), sizeof(T))) {
                    if (condition(temp)) return temp;
                }
                current_pos += sizeof(T);
            }
            return std::nullopt;
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

        // 2. Yıkıcı (Destructor) - Program kapanırken tapuyu mühürle!
        ~dbio() {
            if (m_file.is_open()) {
                sync_ptr_to_disk(); // En son mühürleme burada yapılıyor
                m_file.close();
                log.logf("DB safe closed and synced.");
            }
        }

        void commit() {
            sync_ptr_to_disk();
            log.logf("DB safe committed.");
        }













    };
    };


#endif //MY_STL_DB_IO_H