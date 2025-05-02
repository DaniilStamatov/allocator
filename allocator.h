#include <sys/mman.h>
#include <cstddef>

namespace ds {
    class allocator {
    struct block_header {
        size_t size;
        bool free;
        block_header* next;
        block_header* prev;
    };
    struct free_list {
        block_header* head = nullptr;
        block_header* tail = nullptr;
    };

    public:
        void* allocate (size_t size);
        void dealloc(void* ptr);
    private:
        void remove_from_list(block_header* block);
        void split_block(block_header* block, size_t size);
        block_header* find_fit(size_t requested_size);
    private:
        const static size_t SPLIT_THRESHOLD = alignof(max_align_t);

        free_list m_list;
    };
}
