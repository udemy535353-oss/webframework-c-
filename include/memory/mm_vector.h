//
// Created by halim on 27.02.2026.
//

#ifndef MY_STL_MM_VECTOR_H
#define MY_STL_MM_VECTOR_H

#include <utility>
#include <memory>


namespace nsq {
    template<typename T>
    class vector {
    private:
        T* m_start = nullptr;
        T* m_end = nullptr;
        T* m_endof = nullptr;


        void reallocate(size_t cap) {

            T* new_data = static_cast<T*>(::operator new(cap * sizeof(T)));

            size_t current_size = size();

            for (size_t i = 0;i<current_size;++i) {

                new(&new_data[i]) T(std::move(m_start[i]));
                m_start[i].~T();

            }

            ::operator delete(m_start);

            m_start = new_data;

            m_end = new_data + current_size;

            m_endof = new_data + cap;


        }
    public:


        size_t capacity() {
           return m_endof - m_start;
        }
        size_t size() {
            return m_end - m_start;

        }

        void push_back(const T& value) {

            if (m_endof == m_end) {
                reallocate(capacity() == 0 ? 1: capacity() *2);
            }

            new (m_end) T(value);
            m_end++;


        }

        T& operator[](size_t index) { return m_start[index]; }

        vector() = default;
        ~vector() { clear(); ::operator delete(m_start); }

        void clear() {
            while (m_end != m_start) {
                (--m_end)->~T();
            }
        }
    };

}



#endif //MY_STL_MM_VECTOR_H