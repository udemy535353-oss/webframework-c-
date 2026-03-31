//
// Created by halim on 25.02.2026.
//
#include "memory/mm_arena.h"

namespace nsq {

    uintptr_t  memory::align_forward(uintptr_t ptr, size_t align) {
        return (ptr + (static_cast<uintptr_t>(align) - 1)) & ~(static_cast<uintptr_t>(align) - 1);
    }


     memory::~memory() {

        if (m_ctx.base != nullptr) {
            std::free(m_ctx.base);
            m_ctx.base = nullptr;
        }
    }

    memory::memory(size_t total_size) {
        m_ctx.base = static_cast<uint8_t*>(std::malloc(total_size));
        m_ctx.size = total_size;
        m_ctx.offset = 0;
    }

    void memory::reset() {
        if (m_ctx.base != nullptr) {
            m_ctx.offset=0;
        }
    }

    void* memory::arena_alloc(memory::Arena&arena,size_t size,size_t align) {
        uintptr_t current = reinterpret_cast<uintptr_t>(arena.base) + arena.offset;
        uintptr_t aligned = memory::align_forward(current,align);

        size_t new_offset = (aligned - reinterpret_cast<uintptr_t>(arena.base)) + size;

        if(new_offset > arena.size) {
            return nullptr;
        }

        arena.offset = new_offset;

        return reinterpret_cast<void*>(aligned);
    }

}