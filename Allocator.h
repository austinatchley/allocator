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

#include <cassert> // assert
#include <cmath>
#include <cstddef>   // ptrdiff_t, size_t
#include <new>       // bad_alloc, new
#include <stdexcept> // invalid_argument
#include <unordered_set>

#include <iostream>

// ---------
// Allocator
// ---------

template <typename T, std::size_t N> class my_allocator {
public:
  // --------
  // typedefs
  // --------

  using value_type = T;

  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;

  using pointer = value_type *;
  using const_pointer = const value_type *;

  using reference = value_type &;
  using const_reference = const value_type &;

public:
  // -----------
  // operator ==
  // -----------

  friend bool operator==(const my_allocator &, const my_allocator &) {
    return false;
  } // this is correct

  // -----------
  // operator !=
  // -----------

  friend bool operator!=(const my_allocator &lhs, const my_allocator &rhs) {
    return !(lhs == rhs);
  }

private:
  // ----
  // data
  // ----

  char a[N];

  std::unordered_set<const int *> val_pointers;
  const size_type min_size = sizeof(T) + (2 * sizeof(int));

  // -----
  // valid
  // -----

  /**
   * O(1) in space
   * O(n) in time
   * @return a bool, representing whether or not the allocator is valid
   */
  bool valid() const {
    const int *p = &(*this)[0];

    // while p is not the last sentinel, check if the sentinels are valid
    while (p < &(*this)[N - sizeof(int)]) {
      // if the begin sentinel doesn't match the end sentinel, return false
      if (*p != *(p + std::abs(*p) / sizeof(int) + 1))
        return false;

      // try to find the pointer in the val_pointers set
      if (*p < 0 && val_pointers.find((p + 1)) == val_pointers.end())
        return false;

      p += std::abs(*p) / sizeof(int) + 2;
    }
    return true;
  }

public:
  // ------------
  // constructors
  // ------------

  /**
   * O(1) in space
   * O(1) in time
   * throw a bad_alloc exception, if N is less than sizeof(T) + (2 *
   * sizeof(int))
   */
  my_allocator() : val_pointers(0) {
    if (N < min_size) {
      std::bad_alloc exception;
      throw exception;
    }

    (*this)[0] = N - 8; // N bytes minus 2 sentinels * 4 bytes each
    (*this)[N - sizeof(int)] = N - 8;

    assert(valid());
  }

  my_allocator(const my_allocator &) = default;
  ~my_allocator() = default;
  my_allocator &operator=(const my_allocator &) = default;

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
   * @param num_obj the number of value_types to allocate
   * @return a pointer to the first value in the new block
   */
  pointer allocate(size_type num_obj) {
    if (num_obj <= 0) {
      std::bad_alloc exception;
      throw exception;
    }

    // number of bytes to allocate
    size_type size = num_obj * sizeof(T);

    // initialize p to the first sentinel
    int *p = &(*this)[0];

    // advance p until we find the right block or we hit the end
    while (p != &(*this)[N - sizeof(int)]) {
      if (*p > 0 && static_cast<unsigned int>(*p) >= size) {
        break;
      }

      // add 2 to account for the sentinels
      p += std::abs(*p) / sizeof(int) + 2;
    }

    // if we exit the loop, and the block is too small (or negative)
    // return NULL
    if (*p < 0 || (*p > 0 && static_cast<unsigned int>(*p) < size))
      return 0;

    // pointer to beginning of user's block
    int *new_block = p + 1;

    int free_size = *p;

    // if there isn't enough space left over for another,
    // take the whole block
    if (static_cast<unsigned int>(free_size - size) < min_size) {
      *p *= -1;
      *(p + free_size / sizeof(int) + 1) *= -1;
      val_pointers.insert(new_block);
      return reinterpret_cast<pointer>(new_block);
    }

    int *start_point = p;
    int *end_point = start_point + size / sizeof(int) + 1;
    *p = -size;
    *end_point = -size;

    // only update the following sentinels if we aren't at the end
    if (end_point != &(*this)[N - sizeof(int)]) {
      p = end_point + 1;
      end_point = start_point + free_size / sizeof(int) + 1;

      int new_free_size = free_size - size - (2 * sizeof(int));
      *p = new_free_size;
      *end_point = new_free_size;
    }

    val_pointers.insert(new_block);
    return reinterpret_cast<pointer>(new_block);
  }

  // ---------
  // construct
  // ---------

  /**
   * O(1) in space
   * O(1) in time
   */
  void construct(pointer p, const_reference v) {
    new (p) T(v); // this is correct and exempt
    assert(valid());
  } // from the prohibition of new

  // ----------
  // deallocate
  // ----------

  /**
   * O(1) in space
   * O(1) in time
   * after deallocation adjacent free blocks must be coalesced
   * throw an invalid_argument exception, if p is invalid
   * @param p pointer to the block to deallocate
   * @param size_type unused amount of bytes to deallocate
   */
  void deallocate(pointer p, size_type) {
    int *point = reinterpret_cast<int *>(p);

    if (val_pointers.find(point) == val_pointers.end()) {
      throw std::invalid_argument("invalid arg in deallocate");
    } else
      val_pointers.erase(point);

    int *front = point - 1;
    int *end = front + -1 * (*front) / sizeof(int) + 1;
    int size = -1 * (*front);

    // update our sentinels according to whether or not we
    // should coalesce with other free blocks
    if (front != &(*this)[0] && *(front - 1) > 0) {
      front -= *(front - 1) / sizeof(int) + 2;
      size += *front + 2 * sizeof(int);
    }

    if (end != &(*this)[N - sizeof(int)] && *(end + 1) > 0) {
      end += *(end + 1) / sizeof(int) + 2;
      size += *end + 2 * sizeof(int);
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
  void destroy(pointer p) {
    p->~T(); // this is correct
    assert(valid());
  }

  /**
   * O(1) in space
   * O(1) in time
   */
  int &operator[](int i) { return *reinterpret_cast<int *>(&a[i]); }

  /**
   * O(1) in space
   * O(1) in time
   */
  const int &operator[](int i) const {
    return *reinterpret_cast<const int *>(&a[i]);
  }
};

#endif // Allocator_h
