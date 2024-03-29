// ---------------------------------------
// projects/c++/allocator/RunAllocator.c++
// Copyright (C) 2018
// Glenn P. Downing
// ---------------------------------------

// --------
// includes
// --------

#include <algorithm>
#include <cassert> // assert
#include <deque>
#include <fstream>  // ifstream
#include <iostream> // cout, endl, getline
#include <string>   // s

#include "Allocator.h"

using namespace std;

/*
 * print the sentinels in an allocator
 * @param x the allocator to print
 */
template <typename T, size_t N>
void allocator_print(const my_allocator<T, N> &x) {
  int index = 0;
  while (static_cast<unsigned int>(index) < N - sizeof(int)) {
    cout << x[index] << " ";
    index += std::abs(x[index]) + 2 * sizeof(int);
  }
  cout << endl;
}

/*
 * print each int in an allocator
 * @param x the allocator to print
 */
template <typename T, int N>
void allocator_print_ints(const my_allocator<T, N> &x) {
  int index = 0;
  while (index < N) {
    cout << x[index] << " ";
    index += sizeof(int);
  }
  cout << endl;
}

// ----
// main
// ----

int main() {
  string s;

  getline(cin, s);
  int cases = stoi(s);

  // Blank line
  getline(cin, s);

  while (cases--) {
    my_allocator<double, 1000> x;
    deque<double *> pointers;

    while (getline(cin, s) && s.length() != 0) {
      int val = stoi(s);

      if (val > 0) {
        double *pointer = x.allocate(val);
        auto spot = std::upper_bound(pointers.begin(), pointers.end(), pointer);

        pointers.insert(spot, pointer);
      } else if (val < 0) {
        int index = -val - 1;
        x.deallocate(pointers[index], 0);

        pointers.erase(pointers.begin() + index);
      } else // this should never run
        assert(0);
    }

    allocator_print<double, 1000>(x);
  }
  return 0;
}
