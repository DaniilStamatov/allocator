#include "allocator.h"
#include <iostream>
using namespace ds;

ds::allocator::~allocator() {
    block_header* current = m_list.head;
    while (current != nullptr) {
        block_header* next = current->next;
        munmap(current, current->size);
        current = next;
    }

    m_list.head = nullptr;
    m_list.tail = nullptr;
} 

void* allocator::allocate (size_t size) {
    if(size == 0) {
        return nullptr;
    }
    size_t page_size = 4096;
    size_t total_size = sizeof(block_header) + size; 
    size_t rounded_size = ((total_size + page_size - 1) / page_size) * page_size;

    block_header* block = find_fit(total_size);
    if(block) {
        if(block->size >= total_size + SPLIT_THRESHOLD) split_block(block, total_size);
        block->free = false;
        return block + 1;
    }

    block_header* vptr = reinterpret_cast<block_header*> (mmap(nullptr, total_size,  PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0));
    if(vptr == MAP_FAILED) {
        return nullptr;
    }

    vptr->size = total_size;
    vptr->free = false;
    vptr->prev = nullptr;
    vptr->next = nullptr;
    if(m_list.head) {
        vptr->next = m_list.head;
        m_list.head->prev = vptr;
    } else {
        m_list.tail = vptr;
    }
    m_list.head = vptr;
    return reinterpret_cast<void*> (vptr + 1);
}

void allocator::dealloc(void* ptr) {
    if(!ptr) { return; }
    
    block_header* block = reinterpret_cast<block_header*> (ptr) - 1;
    if(block->free) { return; }

    if (block->next && block->next->free) {
        block->size += block->next->size + sizeof(block_header); 
        block->next = block->next->next;
        if (block->next) {
            block->next->prev = block;
        }
    }

    if (block->prev && block->prev->free) {
        block->prev->size += block->size + sizeof(block_header); 
        block->prev->next = block->next;
        if (block->next) {
            block->next->prev = block->prev;
        }
    }
    size_t total_size = block->size;
    remove_from_list(block);
    munmap(block, total_size);
}

void allocator::remove_from_list(block_header *block)
{
    if(block->prev) {
        block->prev->next = block->next;
    } else {
        m_list.head = block->next;
    }
    
    if(block->next) {
        block->next->prev = block->prev;
    } else {
        m_list.tail = block->prev;
    }
}

void ds::allocator::split_block(block_header *block, size_t requested_size)
{
    size_t remaining_size = block->size - requested_size;

    block_header* new_block = reinterpret_cast<block_header*>(
        reinterpret_cast<char*>(block) + requested_size
    );

    new_block->size = remaining_size;
    new_block->free = true;
    new_block->prev = block;
    new_block->next = block->next;

    if (block->next) {
        block->next->prev = new_block;
    }

    block->next = new_block;
    block->size = requested_size;

    if (m_list.tail == block) {
        m_list.tail = new_block;
    }
}

allocator::block_header* allocator::find_fit(size_t requested_size)
{
    block_header* result = m_list.head;
    while(result != nullptr) {
        if(result->size >= requested_size && result->free) {
            return result;
        } 
        result = result->next;
    }
    return nullptr;
}
