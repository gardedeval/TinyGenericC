# TinyGenericC
Tiny C libraries that exploit undefined behavior to achieve generic programming. Requires C99 support.

Inspired by [rxi/map](https://github.com/rxi/map) and [rxi/vec](https://github.com/rxi/vec) and [attractivechaos/klib](https://github.com/attractivechaos/klib)

# WARNING
You are summoning the horrific nasal demons. Multiple nasal demons. You should never recall them unless you are steer-clear of knowing what you are doing. 

Compile-time type checking is virtually and practically disabled here by the side effect of pointer rewiring. 

I'm planning to import static_assert but it didn't really worked out on MSVC, only the IDE will show the type error.

Documents and comments are not completed. 

THREAD-UNSAFE EVEN WITH LOCKS PLEASE DO NOT USE IT IN PRODUCTION.

# Index
_All libraries are embedded in single-file header form_

Currently undergoing development:
1. [vector(T)](https://github.com/stevefan1999/TinyGenericC/blob/master/include/vector.h): Dynamic array. Header-only.
2. [vector_view(T)](https://github.com/stevefan1999/TinyGenericC/blob/master/include/vector_view.h): Like vector but using a static memory location.
3. [linked_list(T) and singly_linked_list(T)](https://github.com/stevefan1999/TinyGenericC/blob/master/include/linked_list.h): (Type-unsafe) Linked List.
4. [hash_table(K, V)](https://github.com/stevefan1999/TinyGenericC/blob/master/include/hash_table.h): Hash table using FNV-1a and separate chaining with list head cells. Custom hash has to be done by user.
5. [binary_tree(T)](https://github.com/stevefan1999/TinyGenericC/blob/master/include/binary_tree.h): Binary tree (WIP)
6. [ref_count(T)](https://github.com/stevefan1999/TinyGenericC/blob/master/include/ref_count.h): Manual reference counting (WIP). Header-only.
7. [mempool](https://github.com/stevefan1999/TinyGenericC/blob/master/include/mempool.h): Memory pool (WIP). Hopefully header-only.
8. [tagged_mem](https://github.com/stevefan1999/TinyGenericC/blob/master/include/tagged_mem.h): Linear memory with a pre-defined size (just like [sds](https://github.com/antirez/sds)) (WIP)

A toy JSON library is also included here to demonstrate TinyGenericC. However the deserializer part is not started (I'm still learning parser sorry).

# Version history
* 0.0.4: Incorporated static_assert to accommodate compile-time type matching hint 
* 0.0.3a: Added a loop detection algorithm for singly linked list (Floyd's cycle-finding)
* 0.0.3: Reworked on the algorithm of hash_table (Now using a self-invented and flawed twin-hash implementation)
* 0.0.2: Rewriting all the external linkage functions into C99-inline form
* 0.0.1: Init


# Note
Unfortunately, this is not true generic programming. Instead, it's more like Java-ish type erasure that defines a certain bound on object/struct size beforehand, and the program pretends not to write beyond the memory by asserting the size at run-time. 

The sentinel objects to operate on the generic object (e.g. iterator, index) however, are user-defined, and so highly dangerous since type-aliasings are easily or unintentionally violated. 

Therefore, types are not reified and validated. You are responsible for correcting the types in case you shoot yourself in the foot.

# TODO
1. Implement mempool first to support a memory allocator (useful for MCUs)
2. Rewrite all containers to utilize mempool
3. Make vector_view able to use local buffer
4. Fix the goddamn documentation!

# Footnote
If you are a computer science student/lecturer/professor, no, don't kill me. Please don't send me no death threats.
