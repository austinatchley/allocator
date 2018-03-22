// -----------------------------------------
// projects/c++/allocator/TestAllocator1.c++
// Copyright (C) 2017
// Glenn P. Downing
// -----------------------------------------
// https://github.com/google/googletest
// https://github.com/google/googletest/blob/master/googletest/docs/Primer.md
// https://github.com/google/googletest/blob/master/googletest/docs/AdvancedGuide.md

// --------
// includes
// --------

#include <algorithm> // count
#include <memory>    // allocator

#include "gtest/gtest.h"

#include "Allocator.h"

// --------------
// TestAllocator1
// --------------

template <typename A> struct TestAllocator1 : testing::Test {
  // --------
  // typedefs
  // --------

  typedef A allocator_type;
  typedef typename A::value_type value_type;
  typedef typename A::size_type size_type;
  typedef typename A::pointer pointer;
};

typedef testing::Types<std::allocator<int>, std::allocator<double>,
                       my_allocator<int, 100>, my_allocator<double, 100>,
                       my_allocator<long, 100>>
    my_types_1;

TYPED_TEST_CASE(TestAllocator1, my_types_1);

TYPED_TEST(TestAllocator1, test_1) {
  typedef typename TestFixture::allocator_type allocator_type;
  typedef typename TestFixture::value_type value_type;
  typedef typename TestFixture::size_type size_type;
  typedef typename TestFixture::pointer pointer;

  allocator_type x;
  const size_type s = 1;
  const value_type v = 2;
  const pointer p = x.allocate(s);
  if (p != nullptr) {
    x.construct(p, v);
    ASSERT_EQ(v, *p);
    x.destroy(p);
    x.deallocate(p, s);
  }
}

TYPED_TEST(TestAllocator1, test_2) {
  typedef typename TestFixture::allocator_type allocator_type;
  typedef typename TestFixture::value_type value_type;
  typedef typename TestFixture::size_type size_type;
  typedef typename TestFixture::pointer pointer;

  allocator_type x;
  const size_type s = 1;
  const value_type v = 2;
  const value_type v1 = 4;
  const pointer p = x.allocate(s);
  if (p != nullptr) {
    x.construct(p, v);
    ASSERT_EQ(v, *p);
    x.destroy(p);
    x.deallocate(p, s);
    
    x.allocate(s);
    x.construct(p, v1);
    ASSERT_EQ(v1, *p);
    x.destroy(p);
    x.deallocate(p, s);
  }
}

TYPED_TEST(TestAllocator1, test_10) {
  typedef typename TestFixture::allocator_type allocator_type;
  typedef typename TestFixture::value_type value_type;
  typedef typename TestFixture::size_type size_type;
  typedef typename TestFixture::pointer pointer;

  allocator_type x;
  const size_type s = 10;
  const value_type v = 2;
  const pointer b = x.allocate(s);
  if (b != nullptr) {
    pointer e = b + s;
    pointer p = b;
    try {
      while (p != e) {
        x.construct(p, v);
        ++p;
      }
    } catch (...) {
      while (b != p) {
        --p;
        x.destroy(p);
      }
      x.deallocate(b, s);
      throw;
    }
    ASSERT_EQ(s, std::count(b, e, v));
    while (b != e) {
      --e;
      x.destroy(e);
    }
    x.deallocate(b, s);
  }
}

// --------------
// TestAllocator2
// --------------

TEST(TestAllocator2, const_index) {
  const my_allocator<int, 100> x;
  ASSERT_EQ(x[0], 92);
}

TEST(TestAllocator2, index) {
  my_allocator<int, 100> x;
  ASSERT_EQ(x[0], 92);
}

TEST(TestAllocator2, index1) {
  my_allocator<double, 40> x;
  ASSERT_EQ(x[0], 32);
  double *y = x.allocate(1);
  ASSERT_EQ(x[0], -8);
}

TEST(TestAllocator2, coalesce) {
  my_allocator<double, 40> x;
  ASSERT_EQ(x[0], 32);
  double *y = x.allocate(1);
  ASSERT_EQ(x[0], -8);
  x.deallocate(y, 0);
  ASSERT_EQ(x[0], 32);
}

TEST(TestAllocator2, coalesce1) {
  my_allocator<double, 100> x;
  double *y = x.allocate(1);
  double *z = x.allocate(2);
  ASSERT_EQ(x[0], -8);
  x.deallocate(y, 0);
  ASSERT_EQ(x[0], 8);
}

TEST(TestAllocator2, coalesce2) {
  my_allocator<double, 100> x;
  double *y = x.allocate(1);
  double *z = x.allocate(2);
  ASSERT_EQ(x[0], -8);
  x.deallocate(y, 0);
  ASSERT_EQ(x[0], 8);
  x.deallocate(z, 0);
  ASSERT_EQ(x[0], 92);
}

TEST(TestAllocator2, take_rest) {
  my_allocator<double, 1000> x;
  x.allocate(123);
  ASSERT_EQ(x[0], -992);
}

void f() {
  my_allocator<double, 1> x;
}

TEST(TestAllocator2, bad_bytes) {
  ASSERT_THROW(
      f(),
      std::bad_alloc);
}

void g(int i) {
  my_allocator<double, 1> x;
  x.allocate(i);
}

TEST(TestAllocator2, bad_bytes1) {
  ASSERT_THROW(
    g(0),
    std::bad_alloc
  );
}

TEST(TestAllocator2, bad_bytes2) {
  ASSERT_THROW(
    g(-1),
    std::bad_alloc
  );
}

void h() {
  my_allocator<double, 100> x;
  double *y = x.allocate(8);
  x.deallocate(y+1, 0);
}

TEST(TestAllocator2, bad_dealloc) {
  ASSERT_THROW(
    h(),
    std::invalid_argument
  );
}



// --------------
// TestAllocator3
// --------------

template <typename A> struct TestAllocator3 : testing::Test {
  // --------
  // typedefs
  // --------

  typedef A allocator_type;
  typedef typename A::value_type value_type;
  typedef typename A::size_type size_type;
  typedef typename A::pointer pointer;
};

typedef testing::Types<my_allocator<int, 100>, my_allocator<double, 100>>
    my_types_2;

TYPED_TEST_CASE(TestAllocator3, my_types_2);

TYPED_TEST(TestAllocator3, test_1) {
  typedef typename TestFixture::allocator_type allocator_type;
  typedef typename TestFixture::value_type value_type;
  typedef typename TestFixture::size_type size_type;
  typedef typename TestFixture::pointer pointer;

  allocator_type x;
  const size_type s = 1;
  const value_type v = 2;
  const pointer p = x.allocate(s);
  if (p != nullptr) {
    x.construct(p, v);
    ASSERT_EQ(v, *p);
    x.destroy(p);
    x.deallocate(p, s);
  }
}

TYPED_TEST(TestAllocator3, test_10) {
  typedef typename TestFixture::allocator_type allocator_type;
  typedef typename TestFixture::value_type value_type;
  typedef typename TestFixture::size_type size_type;
  typedef typename TestFixture::pointer pointer;

  allocator_type x;
  const size_type s = 10;
  const value_type v = 2;
  const pointer b = x.allocate(s);
  if (b != nullptr) {
    pointer e = b + s;
    pointer p = b;
    try {
      while (p != e) {
        x.construct(p, v);
        ++p;
      }
    } catch (...) {
      while (b != p) {
        --p;
        x.destroy(p);
      }
      x.deallocate(b, s);
      throw;
    }
    ASSERT_EQ(s, std::count(b, e, v));
    while (b != e) {
      --e;
      x.destroy(e);
    }
    x.deallocate(b, s);
  }
}
