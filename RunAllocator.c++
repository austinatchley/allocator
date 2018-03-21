// ---------------------------------------
// projects/c++/allocator/RunAllocator.c++
// Copyright (C) 2018
// Glenn P. Downing
// ---------------------------------------

// --------
// includes
// --------

#include <cassert>  // assert
#include <fstream>  // ifstream
#include <iostream> // cout, endl, getline
#include <string>   // s

#include "Allocator.h"

using namespace std;

template<typename T, int N>
void allocator_print(const my_allocator<T, N>& x) {
    int index = 0;
    while (index < N) {
        cout << x[index] << " ";
        index += x[index] + 2*sizeof(int);
    }
    cout << endl;
}


template<typename T, int N>
void allocator_print_bytes(const my_allocator<T, N>& x) {
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

int main () {
    ifstream f("RunAllocator.in");
    string s;
    
    getline(f, s);
    int cases = stoi(s);

    // Blank line
    getline(f, s);

    while (cases--) {
        my_allocator<double, 1000> x;
        while(getline(f, s) && s.length() != 0) {
            int val = stoi(s);
            if (val > 0)
              x.allocate(val);
        }
        allocator_print<double, 1000>(x);
        cout << endl;
    }
    return 0;}
