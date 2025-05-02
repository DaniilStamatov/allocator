#include "allocator.h"
#include <cassert>
#include <cstring>
int main () {
    ds::allocator a;

    void* p1 = a.allocate(128);
    assert(p1 != nullptr);
    std::memset(p1, 0xAA, 128); 

    void* p2 = a.allocate(256);
    assert(p2 != nullptr);
    assert(p1 != p2);

    a.dealloc(p1);
    a.dealloc(p2);

    void* p3 = a.allocate(64);
    assert(p3 != nullptr);
    std::memset(p3, 0xBB, 64);
    a.dealloc(p3);

    // ds::allocator alloc;
    // void* ptr = alloc.allocate(200);  
    // assert(ptr != nullptr);

    // void* ptr2 = alloc.allocate(20);
    // assert(ptr2 != nullptr);

    // assert(ptr != ptr2);
    // assert(reinterpret_cast<char*>(ptr) + 200 == reinterpret_cast<char*>(ptr2));

    // alloc.dealloc(ptr);
    // alloc.dealloc(ptr2);

    return 0;
}