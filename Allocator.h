// ----------------------------------
// projects/c++/allocator/Allocator.h
// Copyright (C) 2018
// Glenn P. Downing
// ----------------------------------

#ifndef Allocator_h
#define Allocator_h

// --------
// includes
// --------

#include <cassert>   // assert
#include <cstddef>   // ptrdiff_t, size_t
#include <new>       // bad_alloc, new
#include <stdexcept> // invalid_argument

// ---------
// Allocator
// ---------

template <typename T, std::size_t N>
class my_allocator {
    public:
        // --------
        // typedefs
        // --------

        using      value_type = T;

        using       size_type = std::size_t;
        using difference_type = std::ptrdiff_t;

        using       pointer =       value_type*;
        using const_pointer = const value_type*;

        using       reference =       value_type&;
        using const_reference = const value_type&;

    public:
        // -----------
        // operator ==
        // -----------

        friend bool operator == (const my_allocator&, const my_allocator&) {
            return false;}                                                   // this is correct

        // -----------
        // operator !=
        // -----------

        friend bool operator != (const my_allocator& lhs, const my_allocator& rhs) {
            return !(lhs == rhs);}

    private:
        // ----
        // data
        // ----

        char a[N];

        // -----
        // valid
        // -----

        /**
         * O(1) in space
         * O(n) in time
         * <your documentation>
         */
        bool valid () const {
            // <your code>
            return true;}

    public:
        // ------------
        // constructors
        // ------------

        /**
         * O(1) in space
         * O(1) in time
         * throw a bad_alloc exception, if N is less than sizeof(T) + (2 * sizeof(int))
         */
        my_allocator () {
            if(N < sizeof(T) + (2 * sizeof(int))) {
              std::bad_alloc exception;
              throw exception;
            }

            (*this)[0] = N - 8; // N bytes minus 2 sentinels * 4 bytes each
            (*this)[N/sizeof(int) - 1] = N - 8;
            // <your code>
            assert(valid());}

                      my_allocator  (const my_allocator&) = default;
                      ~my_allocator ()                    = default;
        my_allocator& operator =    (const my_allocator&) = default;

        // --------
        // allocate
        // --------

        /**
         * O(1) in space
         * O(n) in time
         * after allocation there must be enough space left for a valid block
         * the smallest allowable block is sizeof(T) + (2 * sizeof(int))
         * choose the first block that fits
         * throw a bad_alloc exception, if n is invalid
         */
        pointer allocate (size_type size) {
            if (size < sizeof(T) + (2 * sizeof(int))) {
              std::bad_alloc exception;
              throw exception;
            }

            int *p = &(*this)[0];

            while (p != &(*this)[N/sizeof(int) - 1]) {
                if (*p > size)
                  break;
                p += *p; 
            }

            if (*p < sizeof(T) + (2 * sizeof(int)))
              return 0;

            int free_size = *p;
            int *end_point = p + size;
            *p = -size;
            *end_point = -size;

            int *new_block = p;

            p += free_size - size + 1;
            end_point += free_size - size + 1;
            *p = -(*p - size);
            *end_point = -(*end_point - size);

            assert(valid());
            return reinterpret_cast<pointer>(new_block);}

        // ---------
        // construct
        // ---------

        /**
         * O(1) in space
         * O(1) in time
         */
        void construct (pointer p, const_reference v) {
            new (p) T(v);                               // this is correct and exempt
            assert(valid());}                           // from the prohibition of new

        // ----------
        // deallocate
        // ----------

        /**
         * O(1) in space
         * O(1) in time
         * after deallocation adjacent free blocks must be coalesced
         * throw an invalid_argument exception, if p is invalid
         * <your documentation>
         */
        void deallocate (pointer, size_type) {
            // <your code>
            assert(valid());}

        // -------
        // destroy
        // -------

        /**
         * O(1) in space
         * O(1) in time
         */
        void destroy (pointer p) {
            p->~T();               // this is correct
            assert(valid());}

        /**
         * O(1) in space
         * O(1) in time
         */
        int& operator [] (int i) {
            return *reinterpret_cast<int*>(&a[i]);}

        /**
         * O(1) in space
         * O(1) in time
         */
        const int& operator [] (int i) const {
            return *reinterpret_cast<const int*>(&a[i]);}};

#endif // Allocator_h
