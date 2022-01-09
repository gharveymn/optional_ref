/** test-bind.cpp
 * Copyright © 2022 Gene Harvey
 *
 * This software may be modified and distributed under the terms
 * of the MIT license. See the LICENSE file for details.
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "test_common.hpp"

#include <string>

struct test_struct_base
{
  GCH_CPP14_CONSTEXPR
  int
  g (void) &
  {
    return 4;
  }

  constexpr
  int
  g (void) const &
  {
    return 5;
  }

  GCH_CPP14_CONSTEXPR
  void
  gv (void) { gv_val = g (); }

  GCH_CPP14_CONSTEXPR
  void
  gv (void) const { }

  int n = 6;
  int gv_val = 0;
};

struct test_struct
  : test_struct_base
{
  GCH_CPP14_CONSTEXPR
  int
  f (void)
  {
    return 1;
  }

  constexpr
  int
  f (void) const
  {
    return 2;
  }

  constexpr
  int
  h (void) const
  {
    return 11;
  }

  int&
  r (void)
  {
    return m;
  }

  GCH_CPP14_CONSTEXPR
  void
  fv (void) { fv_val = f (); }

  GCH_CPP14_CONSTEXPR
  void
  fv (void) const { }

  int m = 3;
  int fv_val = 0;
};

static constexpr
int
test_struct_func (test_struct&)
{
  return 7;
}

static constexpr
int
test_struct_func (const test_struct&)
{
  return 8;
}

static GCH_CPP14_CONSTEXPR
void
test_struct_func_void (test_struct&) { }

static GCH_CPP14_CONSTEXPR
void
test_struct_func_void (const test_struct&) { }

// static
// int
// test_struct_base_func (test_struct_base&)
// {
//   return 9;
// }
//
// static constexpr
// int
// test_struct_base_func (const test_struct_base&)
// {
//   return 10;
// }

// static GCH_CPP14_CONSTEXPR
// void
// test_struct_base_func_void (test_struct_base&) { }

// static GCH_CPP14_CONSTEXPR
// void
// test_struct_base_func_void (const test_struct_base&) { }

static constexpr
int
test_struct_no_overload (const test_struct&)
{
  return 12;
}

static
std::size_t
test_struct_args (const test_struct&, const std::string& s)
{
  return s.size ();
}

int
main (void)
{
  test_struct       ts;
  const test_struct cts;

  gch::optional_ref<test_struct>       opt  { ts  };
  gch::optional_ref<const test_struct> copt { cts };

  CHECK ((opt >>= &test_struct::f)      == ts.f ());
  CHECK ((opt >>= &test_struct::g)      == ts.g ());
  CHECK ((opt >>= &test_struct_base::g) == ts.g ());
  CHECK ((opt >>= &test_struct::h)      == ts.h ());

  CHECK ((opt >>= &test_struct::m)     .refers_to (ts.m));
  CHECK ((opt >>= &test_struct::n)     .refers_to (ts.n));
  CHECK ((opt >>= &test_struct_base::n).refers_to (ts.n));

  opt >>= &test_struct::fv;
  CHECK (*(opt >>= &test_struct::fv_val) == ts.f ());

  opt >>= &test_struct::gv;
  CHECK (*(opt >>= &test_struct::gv_val) == ts.g ());

  CHECK ((copt >>= &test_struct::f)      == cts.f ());
  CHECK ((copt >>= &test_struct::g)      == cts.g ());
  CHECK ((copt >>= &test_struct_base::g) == cts.g ());
  CHECK ((copt >>= &test_struct::h)      == cts.h ());

  CHECK ((copt >>= &test_struct::m)     .refers_to (cts.m));
  CHECK ((copt >>= &test_struct::n)     .refers_to (cts.n));
  CHECK ((copt >>= &test_struct_base::n).refers_to (cts.n));

  copt >>= &test_struct::fv;
  copt >>= &test_struct::gv;
  copt >>= &test_struct_base::gv;

#ifndef GCH_CLANG
  CHECK ((opt >>= test_struct_func)        == test_struct_func (ts));
#endif
  CHECK ((opt >>= test_struct_no_overload) == test_struct_no_overload (ts));

  opt >>= test_struct_func_void;

#ifndef GCH_CLANG
  CHECK ((copt >>= test_struct_func)        == test_struct_func (cts));
#endif
  CHECK ((copt >>= test_struct_no_overload) == test_struct_no_overload (cts));

  copt >>= test_struct_func_void;
  // copt >>= test_struct_base_func_void;

  opt >> [] (void) noexcept { void (0); };

  CHECK (maybe_invoke (opt, test_struct_args, "hi") == 2);

  return 0;
}
