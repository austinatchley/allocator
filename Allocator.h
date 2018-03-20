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

#include <iostream>

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

        int min_size = sizeof(T) + (2 * sizeof(int));

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
            if(N < min_size) {
              std::bad_alloc exception;
              throw exception;
            }

            (*this)[0] = N - 8; // N bytes minus 2 sentinels * 4 bytes each
            (*this)[N - sizeof(int)] = N - 8;

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
        pointer allocate (size_type num_obj) {
            if (num_obj <= 0) {  
              std::bad_alloc exception;
              throw exception;
            }

            // number of bytes to allocate
            size_type size = num_obj * sizeof(T);

            // initialize p to the first sentinel
            int *p = &(*this)[0];

            while (p != &(*this)[N - sizeof(int)]) {
                if (*p > 0 && *p > size) {
                  break;
                }
                p += abs(*p)/sizeof(int) + 1; 
            }

            // if we exit the loop, and the block is too small (or negative)
            // return NULL
            if (*p > 0 && *p < size)
              return 0;

            int *new_block = p + 1;

            int free_size = *p;
            if (free_size - size < min_size) {
              *p *= -1;
              *(p + free_size/sizeof(int) + 1) *= -1;
              return reinterpret_cast<pointer>(new_block);
            }

            int *start_point = p;
            int *end_point = start_point + size/sizeof(int) + 1;
            *p = -size;
            *end_point = -size;

            p = end_point + 1;
            end_point = start_point + free_size/sizeof(int) + 1;

            int new_free_size = free_size - size - (2 * sizeof(int));
            *p = new_free_size;
            *end_point = new_free_size;

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
        void deallocate (pointer p, size_type) {
            assert(valid());
	    int *front = reinterpret_cast<int*>(p) - 1;
	    int *end = front + *front + 1;
	    int size = *front;
	
	    if(front!= reinterpret_cast<int*>(a) && *(front - 1) > 0) {
		front -= *(front - 1) + 2;
		size += *front + 2*sizeof(int);
	    }
	    if(end != reinterpret_cast<int*>(a + N) && *(end + 1) > 0) {
		end+= *(end + 1) + 2;
		size += *end + 2*sizeof(int);

	    }
	    *front = size;
	    *end = size;
	    
	}

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
