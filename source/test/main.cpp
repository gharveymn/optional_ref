#include "gch/optional_ref.hpp"

#include <iostream>
#include <cassert>
#include <unordered_map>
#include <chrono>
#include <random>
#include <algorithm>
#include <numeric>

using namespace gch;

struct base { };
struct derived : base { };

// these cause ICE with MSVC for some reason
#ifndef _MSC_VER

template class gch::optional_ref<int>;

template class gch::optional_ref<base>;
template class gch::optional_ref<derived>;
template gch::optional_ref<base>::optional_ref (base&);
template gch::optional_ref<base>::optional_ref (derived&);
template gch::optional_ref<base>::optional_ref (const optional_ref<derived>&) noexcept;
template auto gch::optional_ref<base>::value_or (base&) const -> reference;
template auto gch::optional_ref<base>::value_or (derived&) const -> reference;
template auto gch::optional_ref<base>::emplace (base&) -> reference;
template auto gch::optional_ref<base>::emplace (derived&) -> reference;
template bool gch::optional_ref<base>::contains (base&) const;
template bool gch::optional_ref<base>::contains (derived&) const;

#endif

template bool gch::operator== (const optional_ref<int>&, const optional_ref<int>&) noexcept;
template bool gch::operator!= (const optional_ref<int>&, const optional_ref<int>&) noexcept;
template bool gch::operator<  (const optional_ref<int>&, const optional_ref<int>&) noexcept;
template bool gch::operator>  (const optional_ref<int>&, const optional_ref<int>&) noexcept;
template bool gch::operator<= (const optional_ref<int>&, const optional_ref<int>&) noexcept;
template bool gch::operator>= (const optional_ref<int>&, const optional_ref<int>&) noexcept;
#ifdef GCH_LIB_THREE_WAY_COMPARISON
template std::strong_ordering gch::operator<=> (const optional_ref<int>&, const optional_ref<int>&) noexcept;
#endif

template bool gch::operator== (const optional_ref<int>&, nullopt_t) noexcept;
#ifdef GCH_LIB_THREE_WAY_COMPARISON
template std::strong_ordering gch::operator<=> (const optional_ref<int>&, nullopt_t) noexcept;
#else
template bool gch::operator== (nullopt_t,                const optional_ref<int>&) noexcept;
template bool gch::operator!= (const optional_ref<int>&, nullopt_t               ) noexcept;
template bool gch::operator!= (nullopt_t,                const optional_ref<int>&) noexcept;
template bool gch::operator<  (const optional_ref<int>&, nullopt_t               ) noexcept;
template bool gch::operator<  (nullopt_t,                const optional_ref<int>&) noexcept;
template bool gch::operator>  (const optional_ref<int>&, nullopt_t               ) noexcept;
template bool gch::operator>  (nullopt_t,                const optional_ref<int>&) noexcept;
template bool gch::operator<= (const optional_ref<int>&, nullopt_t               ) noexcept;
template bool gch::operator<= (nullopt_t,                const optional_ref<int>&) noexcept;
template bool gch::operator>= (const optional_ref<int>&, nullopt_t               ) noexcept;
template bool gch::operator>= (nullopt_t,                const optional_ref<int>&) noexcept;
#endif

template bool gch::operator== (const optional_ref<int>&, std::nullptr_t) noexcept;
#ifdef GCH_LIB_THREE_WAY_COMPARISON
template std::strong_ordering gch::operator<=> (const optional_ref<int>&, std::nullptr_t) noexcept;
#else
template bool gch::operator== (std::nullptr_t,           const optional_ref<int>&) noexcept;
template bool gch::operator!= (const optional_ref<int>&, std::nullptr_t          ) noexcept;
template bool gch::operator!= (std::nullptr_t,           const optional_ref<int>&) noexcept;
template bool gch::operator<  (const optional_ref<int>&, std::nullptr_t          ) noexcept;
template bool gch::operator<  (std::nullptr_t,           const optional_ref<int>&) noexcept;
template bool gch::operator>  (const optional_ref<int>&, std::nullptr_t          ) noexcept;
template bool gch::operator>  (std::nullptr_t,           const optional_ref<int>&) noexcept;
template bool gch::operator<= (const optional_ref<int>&, std::nullptr_t          ) noexcept;
template bool gch::operator<= (std::nullptr_t,           const optional_ref<int>&) noexcept;
template bool gch::operator>= (const optional_ref<int>&, std::nullptr_t          ) noexcept;
template bool gch::operator>= (std::nullptr_t,           const optional_ref<int>&) noexcept;
#endif

template bool gch::operator== (const optional_ref<int>&, const int *             ) noexcept;
template bool gch::operator== (const int *,              const optional_ref<int>&) noexcept;
template bool gch::operator!= (const optional_ref<int>&, const int *             ) noexcept;
template bool gch::operator!= (const int *,              const optional_ref<int>&) noexcept;
template bool gch::operator<  (const optional_ref<int>&, const int *             ) noexcept;
template bool gch::operator<  (const int *,              const optional_ref<int>&) noexcept;
template bool gch::operator>  (const optional_ref<int>&, const int *             ) noexcept;
template bool gch::operator>  (const int *,              const optional_ref<int>&) noexcept;
template bool gch::operator<= (const optional_ref<int>&, const int *             ) noexcept;
template bool gch::operator<= (const int *,              const optional_ref<int>&) noexcept;
template bool gch::operator>= (const optional_ref<int>&, const int *             ) noexcept;
template bool gch::operator>= (const int *,              const optional_ref<int>&) noexcept;
#ifdef GCH_LIB_THREE_WAY_COMPARISON
template std::strong_ordering gch::operator<=> (const optional_ref<int>&, const int *) noexcept;
#endif

static_assert (std::is_trivially_copy_constructible<optional_ref<int>>::value, "");
static_assert (std::is_trivially_move_constructible<optional_ref<int>>::value, "");
static_assert (std::is_trivially_copy_assignable<optional_ref<int>>::value, "");
static_assert (std::is_trivially_move_assignable<optional_ref<int>>::value, "");
static_assert (std::is_trivially_destructible<optional_ref<int>>::value, "");
static_assert (std::is_trivially_copyable<optional_ref<int>>::value, "");

static constexpr int g_x = 0;
static constexpr optional_ref<const int> g_rx { g_x };

void print_test_header (const std::string& str)
{
  std::cout << str << "... ";
}

void print_test_footer ()
{
  std::cout << "pass" << std::endl;
}

void test_const (void)
{
  print_test_header ("test const");

  int x = 1;
  int y = 2;
  const int z = 1;

  optional_ref<int> rx { x };
  optional_ref<const int> rz;
  assert (rx.has_value ());
  assert (bool (rx));
  assert (! rz.has_value ());
  assert (! rz);
  assert (rx != rz);

  // set rz
  rz = z;

  assert (rx != rz);
  assert (rx.get_pointer () != rz.get_pointer ());

  // set rz with a non-const reference
  rz = y;

  assert (rx != rz);
  assert (rx.get_pointer () != rz.get_pointer ());

  rz = x;

  assert (rx == rz);
  assert (rx.get_pointer () == rz.get_pointer ());

  print_test_footer ();
}

void test_arrow (void)
{
  print_test_header ("test arrow");

  struct my_struct
  {
    int x;
  } s { 1 };

  const optional_ref<my_struct> r { s };
  assert (r->x == 1);

  print_test_footer ();
}

void test_throw (void)
{
  print_test_header ("test throw");

  const optional_ref<int> r;

  try
  {
    static_cast<void> (r.value ());
  }
  catch (const bad_optional_access& e) { static_cast<void> (e.what ()); }
  catch (...) { throw std::runtime_error ("optional_ref does not throw bad_optional_access"); }

  print_test_footer ();
}

void test_assign (void)
{
  print_test_header ("test assign");

  int x = 1;
  optional_ref<int> r { x };
  assert (r == &x);
  *r = 2;
  assert (r == &x);

  int y = 3;
  assert (r.emplace (y) == 3);
  assert (r == &y);
  static_cast<void> (y);

  print_test_footer ();
}

void test_nullopt (void)
{
  print_test_header ("test nullopt");

  optional_ref<int> r0 = nullopt;
  assert (! r0.has_value ());

  optional_ref<int> r1 = { };
  assert (! r1.has_value ());

  optional_ref<int> r2 (nullopt);
  assert (! r2.has_value ());

  int x = 1;
  int y = 2;
  r0 = x;
  assert (r0.has_value ());
  static_cast<void> (y);

  r1 = x;
  assert (r1.has_value ());

  r2 = x;
  assert (r2.has_value ());

  assert (r0.value () == x);
  assert (r0.value_or (y) == x);
  assert (r0.value_or (y) == 1);
  assert (r0.value_or (2) == 1);

  r0 = nullopt;
  assert (! r0.has_value ());

  r1 = { };
  assert (! r1.has_value ());

  r2.reset ();
  assert (! r2.has_value ());

  assert (r0.value_or (y) == y);
  assert (r0.value_or (y) == 2);
  assert (r0.value_or (2) == 2);

  print_test_footer ();
}

void test_inheritence (void)
{
  print_test_header ("test inheritence");
  struct my_struct_base
  {
    int x;
    constexpr bool operator== (my_struct_base& other) const noexcept
    {
      return this->x == other.x;
    }

    constexpr bool operator!= (my_struct_base& other) const noexcept
    {
      return ! operator== (other);
    }
  };

  struct my_struct : my_struct_base
  { };

  my_struct s0 { };
  my_struct s1 { };
  s1.x = 1;
  const optional_ref<my_struct_base> r0 = s0;
  const optional_ref<my_struct> r1 (s1);

  assert (r0 != r1);
  assert (r0.get_pointer () != r1.get_pointer ());

  const optional_ref<my_struct_base> r2 (r1);

  assert (r2 == r1);
  assert (r2.get_pointer () == r1.get_pointer ());

  print_test_footer ();
}

void test_movement (void)
{
  print_test_header ("test movement");

  int x = 1;
  int y = 2;
  const optional_ref<int> rx { x };
  const optional_ref<int> ry { y };
  assert (! (rx == ry));
  assert (  (rx != ry));

  // copy constructor
  optional_ref<int> rz { rx };
  assert (rz == rx);
  assert (rz != ry);
  assert (rz.get_pointer () == rx.get_pointer ());
  assert (rz.get_pointer () != ry.get_pointer ());

  // move constructor
  optional_ref<int> rm { std::move (rz) };
  assert (rm == rx);
  assert (rm != ry);
  assert (rm.get_pointer () == rx.get_pointer ());
  assert (rm.get_pointer () != ry.get_pointer ());

  // copy assignment operator
  rz = ry;
  assert (rz != rx);
  assert (rz == ry);
  assert (rz.get_pointer () != rx.get_pointer ());
  assert (rz.get_pointer () == ry.get_pointer ());

  // move assignment operator
  rm = std::move (rz);
  assert (rm != rx);
  assert (rm == ry);
  assert (rm.get_pointer () != rx.get_pointer ());
  assert (rm.get_pointer () == ry.get_pointer ());

  // creation using a temporary
  const optional_ref<int> rt { optional_ref<int> { x } };
  assert (rt == &x);
  assert (rt != &y);

  // swap

  optional_ref<int> rp { x };
  optional_ref<int> rq { y };

  using std::swap;
  swap (rp, rq);

  assert (! (rp == rq));
  assert (  (rp != rq));

  print_test_footer ();
}

void test_comparison (void)
{
  print_test_header ("test comparison");
  int a[2] = { 11, 22 };
  const optional_ref<int> rx (a[0]);
  const optional_ref<int> ry (a[1]);

  // not equal
  assert (! (rx == ry));
  assert (! (ry == rx));
  assert (  (rx != ry));
  assert (  (ry != rx));
  assert (  (rx <  ry));
  assert (! (ry <  rx));
  assert (! (rx >  ry));
  assert (  (ry >  rx));
  assert (  (rx <= ry));
  assert (! (ry <= rx));
  assert (! (rx >= ry));
  assert (  (ry >= rx));

  // set value to be equal (should still not be equal in comparison)
  *ry = 11;
  assert (! (rx == ry));
  assert (! (ry == rx));
  assert (  (rx != ry));
  assert (  (ry != rx));
  assert (  (rx <  ry));
  assert (! (ry <  rx));
  assert (! (rx >  ry));
  assert (  (ry >  rx));
  assert (  (rx <= ry));
  assert (! (ry <= rx));
  assert (! (rx >= ry));
  assert (  (ry >= rx));

  // nullopt comparisons (rz is not nullopt)
  const optional_ref<int> rz (rx);
  assert (! (rz      == nullopt));
  assert (! (nullopt == rz     ));
  assert (  (rz      != nullopt));
  assert (  (nullopt != rz     ));
  assert (! (rz      <  nullopt));
  assert (  (nullopt <  rz     ));
  assert (  (rz      >  nullopt));
  assert (! (nullopt >  rz     ));
  assert (! (rz      <= nullopt));
  assert (  (nullopt <= rz     ));
  assert (  (rz      >= nullopt));
  assert (! (nullopt >= rz     ));

  assert (! (rz      == nullptr));
  assert (! (nullptr == rz     ));
  assert (  (rz      != nullptr));
  assert (  (nullptr != rz     ));
  assert (! (rz      <  nullptr));
  assert (  (nullptr <  rz     ));
  assert (  (rz      >  nullptr));
  assert (! (nullptr >  rz     ));
  assert (! (rz      <= nullptr));
  assert (  (nullptr <= rz     ));
  assert (  (rz      >= nullptr));
  assert (! (nullptr >= rz     ));

  // set nullopt
  const optional_ref<int> rn = { };

  // same as above, except rn contains nullopt this time
  assert (! (rx == rn));
  assert (! (rn == rx));
  assert (  (rx != rn));
  assert (  (rn != rx));
  assert (! (rx <  rn));
  assert (  (rn <  rx));
  assert (  (rx >  rn));
  assert (! (rn >  rx));
  assert (! (rx <= rn));
  assert (  (rn <= rx));
  assert (  (rx >= rn));
  assert (! (rn >= rx));

  // rn is still nullopt
  assert (  (rn      == nullopt));
  assert (  (nullopt == rn     ));
  assert (! (rn      != nullopt));
  assert (! (nullopt != rn     ));
  assert (! (rn      <  nullopt));
  assert (! (nullopt <  rn     ));
  assert (! (rn      >  nullopt));
  assert (! (nullopt >  rn     ));
  assert (  (rn      <= nullopt));
  assert (  (nullopt <= rn     ));
  assert (  (rn      >= nullopt));
  assert (  (nullopt >= rn     ));

  assert (  (rn      == nullptr));
  assert (  (nullptr == rn     ));
  assert (! (rn      != nullptr));
  assert (! (nullptr != rn     ));
  assert (! (rn      <  nullptr));
  assert (! (nullptr <  rn     ));
  assert (! (rn      >  nullptr));
  assert (! (nullptr >  rn     ));
  assert (  (rn      <= nullptr));
  assert (  (nullptr <= rn     ));
  assert (  (rn      >= nullptr));
  assert (  (nullptr >= rn     ));

  // compare two optional_refs which are both nullopt
  const optional_ref<int> rm (nullopt);
  assert (  (rn == rm));
  assert (  (rm == rn));
  assert (! (rn != rm));
  assert (! (rm != rn));
  assert (! (rn <  rm));
  assert (! (rm <  rn));
  assert (! (rn >  rm));
  assert (! (rm >  rn));
  assert (  (rn <= rm));
  assert (  (rm <= rn));
  assert (  (rn >= rm));
  assert (  (rm >= rn));

  // compare with a pointer (not equal)
  int *py = &a[1];
  assert (! (rx == py));
  assert (! (py == rx));
  assert (  (rx != py));
  assert (  (py != rx));
  assert (  (rx <  py));
  assert (! (py <  rx));
  assert (! (rx >  py));
  assert (  (py >  rx));
  assert (  (rx <= py));
  assert (! (py <= rx));
  assert (! (rx >= py));
  assert (  (py >= rx));
  static_cast<void> (py);

  // compare with a pointer (equal)
  int *px = &a[0];
  assert (  (rx == px));
  assert (  (px == rx));
  assert (! (rx != px));
  assert (! (px != rx));
  assert (! (rx <  px));
  assert (! (px <  rx));
  assert (! (rx >  px));
  assert (! (px >  rx));
  assert (  (rx <= px));
  assert (  (px <= rx));
  assert (  (rx >= px));
  assert (  (px >= rx));
  static_cast<void> (px);

  // compare pointer with optional_ref which is nullopt
  assert (! (rn == py));
  assert (! (py == rn));
  assert (  (rn != py));
  assert (  (py != rn));
  assert (  (rn <  py));
  assert (! (py <  rn));
  assert (! (rn >  py));
  assert (  (py >  rn));
  assert (  (rn <= py));
  assert (! (py <= rn));
  assert (! (rn >= py));
  assert (  (py >= rn));

  print_test_footer ();
}

void test_make_optional_ref (void)
{
  print_test_header ("test make_optional_ref");

  int x = 1;
  const int y = 2;
  optional_ref<int> rx = make_optional_ref (x);
  optional_ref<const int> ry = make_optional_ref (y);
  assert (rx != ry);

  print_test_footer ();
}

void test_hash (void)
{
  print_test_header ("test hash");

  std::unordered_map<optional_ref<int>, const std::string *> map { };
  int x = 1;
  int y = 2;
  int z = 3;
  std::string xs = "x";
  std::string ys = "y";
  std::string zs = "z";
  map.emplace (x, &xs);
  map.emplace (y, &ys);
  map.emplace (z, &zs);

  assert (&xs == map[x]);
  assert (&ys == map[y]);
  assert (&zs == map[z]);

  print_test_footer ();
}

void test_contains (void)
{
  print_test_header ("test contains");

  int x = 1;
  int y = 2;

  optional_ref<int> rx (x);
  optional_ref<int> ry (y);

  optional_ref<int> rx1 (x);

  assert (  rx.contains (x));
  assert (! rx.contains (y));

  assert (! rx.contains (*ry));
  assert (  rx.contains (*rx1));

  print_test_footer ();
}

void test_deduction (void)
{
#if __cpp_deduction_guides >= 201703
  print_test_header ("test deduction");

  int  x = 1;
  long y = 2;

  optional_ref rx (x);
  optional_ref ry (y);

  print_test_footer ();
#endif
}

double test_perf_equality_worker (void)
{
  using namespace std::chrono;
  using clock = high_resolution_clock;
  using time = clock::time_point;
  time t1 = clock::now ();

  constexpr auto v_sz = 100;
  constexpr auto num  = 10000000;

  std::vector<double> v (v_sz);
  std::generate (v.begin (), v.end (), rand);

  std::random_device rd;
  std::mt19937 gen (rd ());
  std::uniform_int_distribution<std::size_t> dist (0, v_sz - 1);

  optional_ref<double> l;
  optional_ref<double> r;
  for (auto i = 0; i < num; ++i)
  {
    l = v[dist (gen)];
    r = v[dist (gen)];
    static_cast<void> (l == r);
    static_cast<void> (l != r);
    static_cast<void> (l <  r);
    static_cast<void> (l <= r);
    static_cast<void> (l >  r);
    static_cast<void> (l >= r);
  }

  time t2 = clock::now ();
  return duration_cast<duration<double>> (t2 - t1).count ();
}

void test_perf_equality (void)
{
  print_test_header ("test perf equality");
  std::cout << std::endl;

  auto num_tests = 10;
  std::vector<double> times (num_tests);
  std::generate (times.begin (), times.end (), test_perf_equality_worker);
  auto mean = std::accumulate (times.begin (), times.end (), 0.0) / num_tests;

  auto variance = std::accumulate (times.begin (), times.end (), 0.0,
    [&mean, &num_tests] (double accum, double val)
    {
      return accum + (std::pow (val - mean, 2.0) / (num_tests - 1));
    });

  auto stddev = std::sqrt (variance);

  std::cout << "  mean: " << mean << std::endl;
  std::cout << "stddev: " << stddev << std::endl;

  print_test_footer ();
}

void test_pointer_cast (void)
{
  print_test_header ("test pointer cast");

  int  x     = 1;
  int* x_ptr = &x;
  int* n_ptr = nullptr;

  const optional_ref<int> opt_x = x_ptr;
  assert (opt_x.has_value ());
  assert (opt_x.contains (x));

  const optional_ref<int> opt_n = n_ptr;
  assert (! opt_n.has_value ());

  /* implicitly convertible */
  struct imp_conv
  {
    /* implicit */ operator int* (void) noexcept { return m_ptr; }
    int* m_ptr;
  } imp { &x };

  const optional_ref<int> opt_y = imp;
  assert (opt_y.has_value ());
  assert (opt_y.contains (x));

  /* explicitly convertible */
  struct exp_conv
  {
    explicit operator int* (void) noexcept { return m_ptr; }
    int* m_ptr;
  } exp { &x };

  const optional_ref<int> opt_z = optional_ref<int> { exp };
  assert (opt_z.has_value ());
  assert (opt_z.contains (x));

  print_test_footer ();
}

int main (void)
{
  static_cast<void> (g_rx);
  test_const ();
  test_arrow ();
  test_throw ();
  test_assign ();
  test_nullopt ();
  test_inheritence ();
  test_movement ();
  test_comparison ();
  test_make_optional_ref ();
  test_hash ();
  test_contains ();
  test_deduction ();
  // test_perf_equality ();
  test_pointer_cast ();
  return 0;
}
