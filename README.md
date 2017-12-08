# TinyGenericC
Tiny C Libraries that exploits undefined behaviors to achieve generic programming

# WARNING
You are summoning the horrific nasal demons. Multiple nasal demons. You should never recall them unless you are steer-clear of knowing what you are doing. 

Currently undergoing development:
1. vector(T): Dynamic array. Translation-unit-free.
2. vector_view(T): Like vector but using a static memory location. Translation-unit-free.
3. linked_list(T) and singly_linked_list(T): (Type-unsafe) Linked List. Translation-unit-free.
4. hash_table(K, V): Hash table using FNV-1a and separate chaining with list head cells
5. binary_tree(T): Binary tree (WIP)
6. ref_count(T): Manual reference counting (WIP)
7. mempool: Memory pool (WIP)
8. tagged_mem: Linear memory with a pre-defined size (just like [sds](https://github.com/antirez/sds)) (WIP)

A toy JSON library is also included here to demonstrate TinyGenericC. However the deserializer part is not started (I'm still learning parser sorry).

# Note
Unfortunately, this is not truly-generic programming. Instead, it's more like Java-ish type erasure that defined a certain bound on object/struct size beforehand, and the program is pretending to not writing beyond the memory by asserting the size in run-time. 

The object to operate on the generic object (e.g. iterator, index) however, is user-defined, and so highly dangerous. 

Compile-time type checking is practically disabled here by the side effect of pointer rewiring. Types are not retified and validated. You are responsible for correcting the types in case you shot yourself in the foot.
