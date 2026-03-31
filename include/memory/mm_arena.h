//
// Created by halim on 25.02.2026.
//

#ifndef MY_STL_MM_ARENA_H
#define MY_STL_MM_ARENA_H
#include <cstdint>
#include <cstdlib>
#include <utility>
#include <new>
#include <iostream>

namespace nsq {

    template<typename T>
    class ArenaPtr {
        T* m_ptr;
    public:
        explicit ArenaPtr(T* ptr) : m_ptr(ptr) {}
        ~ArenaPtr() { if (m_ptr) m_ptr->~T(); }


        T* operator->() const { return m_ptr; }
        T& operator*() const { return *m_ptr; }
        T* get() const { return m_ptr; }


        ArenaPtr(const ArenaPtr&) = delete;
        ArenaPtr& operator=(const ArenaPtr&) = delete;
        ArenaPtr(ArenaPtr&& other) noexcept : m_ptr(other.m_ptr) { other.m_ptr = nullptr; }
    };

    class memory {

    public:





        ~memory();


        struct Arena {
            uint8_t *base;
            size_t size;
            size_t offset;

        };
        explicit memory(size_t total_size);


        [[nodiscard]] static uintptr_t align_forward(uintptr_t ptr,size_t align);







        void reset();


        [[nodiscard]] static void* arena_alloc(Arena&arena,size_t size,size_t align);




        template<typename T, typename... Args>
        ArenaPtr<T> arena_new(Args&&... args) {


            void* mem = arena_alloc(m_ctx, sizeof(T), alignof(T));

            if (!mem) return ArenaPtr<T>(nullptr);

            T* obj = new (mem) T(std::forward<Args>(args)...);


            return ArenaPtr<T>(obj);
        }
    private:
        Arena m_ctx{nullptr,0,0};



    };

}
#endif //MY_STL_MM_ARENA_H