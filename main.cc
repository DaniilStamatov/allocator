#include "allocator.h"
#include <cassert>
#include <cstring>
#include <iostream>
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
    ds::allocator alloc;

    void* ptr1 = alloc.allocate(100); 
    void* ptr2 = alloc.allocate(200); 
    alloc.dealloc(ptr1);
    alloc.dealloc(ptr2);

    void* ptr3 = alloc.allocate(100); 
    void* ptr4 = alloc.allocate(200); 

    assert(ptr3 != nullptr);
    assert(ptr4 != nullptr);
    std::cout << ptr3  << ": "<< ptr1 << std::endl;
    assert(ptr3 == ptr1);  
    assert(ptr4 == ptr2);  
    std::cout << ptr4  << ": "<< ptr2 << std::endl;

    std::cout << "Allocation and reuse test passed!" << std::endl;

    return 0;
}
