cmake_minimum_required(VERSION 3.20)
project(my_STL LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---------------------------------------------------------------------------
# 1. SİHİRLİ FONKSİYON: TÜM ALT KLASÖRLERİ BUL (Recursive)
# ---------------------------------------------------------------------------
macro(get_subdirectories_recursive result curdir)
    file(GLOB_RECURSE children RELATIVE ${curdir} ${curdir}/*)
    set(dirlist "")
    foreach(child ${children})
        if(IS_DIRECTORY ${curdir}/${child})
            list(APPEND dirlist ${curdir}/${child})
        endif()
    endforeach()
    list(REMOVE_DUPLICATES dirlist)
    set(${result} ${dirlist})
endmacro()

get_subdirectories_recursive(INC_SUBDIRS "${CMAKE_CURRENT_SOURCE_DIR}/include")
get_subdirectories_recursive(SRC_SUBDIRS "${CMAKE_CURRENT_SOURCE_DIR}/src")

# ---------------------------------------------------------------------------
# 2. TÜM KAYNAK VE HEADER DOSYALARINI TARA
# ---------------------------------------------------------------------------
file(GLOB_RECURSE ALL_HEADERS CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
)
file(GLOB_RECURSE ALL_SOURCES CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

# ---------------------------------------------------------------------------
# 3. MARIADB YAPILANDIRMASI (L11 Smart Setup)
# ---------------------------------------------------------------------------
set(MARIADB_ROOT "C:/Program Files/MariaDB 12.1")
set(MARIADB_INCLUDE "${MARIADB_ROOT}/include/mysql")
set(MARIADB_LIB "${MARIADB_ROOT}/lib/libmariadb.lib")
set(MARIADB_DLL "${MARIADB_ROOT}/lib/libmariadb.dll")

# ---------------------------------------------------------------------------
# 4. KÜTÜPHANEYİ OLUŞTUR VE YOLLARI BAĞLA
# ---------------------------------------------------------------------------
add_library(my_stl STATIC ${ALL_SOURCES} ${ALL_HEADERS})

target_include_directories(my_stl PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
        ${INC_SUBDIRS}
        ${SRC_SUBDIRS}
        "${MARIADB_INCLUDE}" # MariaDB Header'larını otomatik ekle
)

# Win32 Bağımlılıklarını (WinSock + MariaDB) kütüphaneye mühürle
if(WIN32)
    target_link_libraries(my_stl PUBLIC ws2_32 "${MARIADB_LIB}")
endif()

# ---------------------------------------------------------------------------
# 5. OTOMATİK EXECUTABLE OLUŞTURMA VE DLL KOPYALAMA
# ---------------------------------------------------------------------------
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    macro(create_auto_exes DIRECTORY_NAME PREFIX)
        file(GLOB_RECURSE FILES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/${DIRECTORY_NAME}/*.cpp")
        foreach(file_path ${FILES})
            get_filename_component(file_name ${file_path} NAME_WE)
            set(target_name "${PREFIX}_${file_name}")

            if(NOT TARGET ${target_name})
                add_executable(${target_name} ${file_path})
                target_link_libraries(${target_name} PRIVATE my_stl)

                # Çıktı klasörünü bin yap
                set_target_properties(${target_name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

                # --- L11 SMART DLL COPY: Derlemeden sonra DLL'i exe'nin yanına atar ---
                if(EXISTS "${MARIADB_DLL}")
                    add_custom_command(TARGET ${target_name} POST_BUILD
                            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                            "${MARIADB_DLL}"
                            $<TARGET_FILE_DIR:${target_name}>
                            COMMENT "L11: libmariadb.dll otomatik kopyalaniyor..."
                    )
                endif()
            endif()
        endforeach()
    endmacro()

    create_auto_exes("tests" "test")
    create_auto_exes("benchmarks" "bench")
endif()