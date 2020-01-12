#include <optional_ref.hpp>

#include <iostream>
#include <cassert>
#include <unordered_map>

using namespace gch;

template struct gch::optional_ref<int>;

static_assert (std::is_trivially_copy_constructible<optional_ref<int>>::value, "");
static_assert (std::is_trivially_move_constructible<optional_ref<int>>::value, "");
static_assert (std::is_trivially_copy_assignable<optional_ref<int>>::value, "");
static_assert (std::is_trivially_move_assignable<optional_ref<int>>::value, "");
static_assert (std::is_trivially_destructible<optional_ref<int>>::value, "");

static constexpr int g_x = 0;

#if __cpp_lib_addressof_constexpr
static constexpr optional_ref<const int> g_rx { g_x };
#else
static optional_ref<const int> g_rx { g_x };
#endif

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
  
  assert (rx == rz);
  assert (rx.operator-> () != rz.operator-> ());
  
  // set rz with a non-const reference
  rz = y;
  
  assert (rx != rz);
  assert (rx.operator-> () != rz.operator-> ());
  
  rz = x;
  
  assert (rx == rz);
  assert (rx.operator-> () == rz.operator-> ());
  
  print_test_footer ();
}

void test_arrow (void)
{
  print_test_header ("test arrow");
  
  struct my_struct
  {
    int x;
  } s { 1 };
  
  optional_ref<my_struct> r { s };
  assert (r->x == 1);
  
  print_test_footer ();
}

void test_throw (void)
{
  print_test_header ("test throw");
  
  optional_ref<int> r;
  
  try
  {
    int& x = r.value ();    
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
  assert (r == 1);
  *r = 2;
  assert (r == 2);
  
  int y = 3;
  assert (r.emplace (y) == 3);
  assert (r == 3);
  
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
  
  r1 = x;
  assert (r1.has_value ());
  
  r2 = x;
  assert (r2.has_value ());
  
  assert (r0.value () == x);
  assert (r0.value_or (y) == x);
  assert (r0.value_or (y) == 1);
  
  r0 = nullopt;
  assert (! r0.has_value ());
  
  r1 = { };
  assert (! r1.has_value ());
  
  r2.reset ();
  assert (! r2.has_value ());
  
  assert (r0.value_or (y) == y);
  assert (r0.value_or (y) == 2);
  
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
  optional_ref<my_struct_base> r0 = s0;
  optional_ref<my_struct> r1 (s1);
  
  assert (r0 != r1);
  assert (r0.operator-> () != r1.operator-> ());
  
  optional_ref<my_struct_base> r2 (r1);
  
  assert (r2 == r1);
  assert (r2.operator-> () == r1.operator-> ());
  
  print_test_footer ();
}

void test_movement (void)
{
  print_test_header ("test movement");
  
  int x = 1;
  int y = 2;
  optional_ref<int> rx { x };
  optional_ref<int> ry { y };
  assert (! (rx == ry));
  assert (rx != ry);
  assert (rx < ry);
  assert (ry > rx);
  assert (rx <= ry);
  assert (ry >= rx);
  
  using std::swap;
  swap (rx, ry);
  
  assert (! (rx == ry));
  assert (rx != ry);
  assert (! (rx < ry));
  assert (! (ry > rx));
  assert (! (rx <= ry));
  assert (! (ry >= rx));
  
  // copy constructor
  optional_ref<int> rz { rx };
  assert (rz == rx);
  assert (rz != ry);
  assert (rz.operator-> () == rx.operator-> ());
  assert (rz.operator-> () != ry.operator-> ());
  
  // move constructor
  optional_ref<int> rm { std::move (rz) };
  assert (rm == rx);
  assert (rm != ry);
  assert (rm.operator-> () == rx.operator-> ());
  assert (rm.operator-> () != ry.operator-> ());
  
  // copy assignment operator
  rz = ry;
  assert (rz != rx);
  assert (rz == ry);
  assert (rz.operator-> () != rx.operator-> ());
  assert (rz.operator-> () == ry.operator-> ());
  
  // move assignment operator
  rm = std::move (rz);
  assert (rm != rx);
  assert (rm == ry);
  assert (rm.operator-> () != rx.operator-> ());
  assert (rm.operator-> () == ry.operator-> ());
  
  print_test_footer ();
}

void test_comparison (void)
{
  print_test_header ("test comparison");
  
  int  x = 11;
  long y = 22;
  optional_ref<int>  rx (x);
  optional_ref<long> ry (y);
  
  // disparate type comparisons (not equal)
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
  
  // disparate type comparisons (equal)
  *ry = 11;
  assert (  (rx == ry));
  assert (  (ry == rx));
  assert (! (rx != ry));
  assert (! (ry != rx));
  assert (! (rx <  ry));
  assert (! (ry <  rx));
  assert (! (rx >  ry));
  assert (! (ry >  rx));
  assert (  (rx <= ry));
  assert (  (ry <= rx));
  assert (  (rx >= ry));
  assert (  (ry >= rx));
  
  // nullopt comparisons
  optional_ref<int> rz (rx);
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
  
  // set nullopt
  rz.reset ();
  // same as above
  assert (! (rx == rz));
  assert (! (rz == rx));
  assert (  (rx != rz));
  assert (  (rz != rx));
  assert (! (rx <  rz));
  assert (  (rz <  rx));
  assert (  (rx >  rz));
  assert (! (rz >  rx));
  assert (! (rx <= rz));
  assert (  (rz <= rx));
  assert (  (rx >= rz));
  assert (! (rz >= rx));
  
  // rz is still nullopt
  assert (  (rz      == nullopt));
  assert (  (nullopt == rz     ));
  assert (! (rz      != nullopt));
  assert (! (nullopt != rz     ));
  assert (! (rz      <  nullopt));
  assert (! (nullopt <  rz     ));
  assert (! (rz      >  nullopt));
  assert (! (nullopt >  rz     ));
  assert (  (rz      <= nullopt));
  assert (  (nullopt <= rz     ));
  assert (  (rz      >= nullopt));
  assert (  (nullopt >= rz     ));
  
  // compare with a value of different type (not equal)
  long vy = 22;
  assert (! (rx == vy));
  assert (! (vy == rx));
  assert (  (rx != vy));
  assert (  (vy != rx));
  assert (  (rx <  vy));
  assert (! (vy <  rx));
  assert (! (rx >  vy));
  assert (  (vy >  rx));
  assert (  (rx <= vy));
  assert (! (vy <= rx));
  assert (! (rx >= vy));
  assert (  (vy >= rx));
  
  // compare with a value of different type (equal)
  vy = 11;
  assert (  (rx == vy));
  assert (  (vy == rx));
  assert (! (rx != vy));
  assert (! (vy != rx));
  assert (! (rx <  vy));
  assert (! (vy <  rx));
  assert (! (rx >  vy));
  assert (! (vy >  rx));
  assert (  (rx <= vy));
  assert (  (vy <= rx));
  assert (  (rx >= vy));
  assert (  (vy >= rx));
  
  // compare with rvalue reference (not equal)
  assert (! (rx == 22));
  assert (! (22 == rx));
  assert (  (rx != 22));
  assert (  (22 != rx));
  assert (  (rx <  22));
  assert (! (22 <  rx));
  assert (! (rx >  22));
  assert (  (22 >  rx));
  assert (  (rx <= 22));
  assert (! (22 <= rx));
  assert (! (rx >= 22));
  assert (  (22 >= rx));
  
  // compare with rvalue reference (equal)
  assert (  (rx == 11));
  assert (  (11 == rx));
  assert (! (rx != 11));
  assert (! (11 != rx));
  assert (! (rx <  11));
  assert (! (11 <  rx));
  assert (! (rx >  11));
  assert (! (11 >  rx));
  assert (  (rx <= 11));
  assert (  (11 <= rx));
  assert (  (rx >= 11));
  assert (  (11 >= rx));
  
  // compare with optional_ref which is nullopt
  rz.reset ();
  assert (! (rz == vy));
  assert (! (vy == rz));
  assert (  (rz != vy));
  assert (  (vy != rz));
  assert (  (rz <  vy));
  assert (! (vy <  rz));
  assert (! (rz >  vy));
  assert (  (vy >  rz));
  assert (  (rz <= vy));
  assert (! (vy <= rz));
  assert (! (rz >= vy));
  assert (  (vy >= rz));
  
  // compare two optional_refs which are both nullopt
  optional_ref<long> rn (nullopt);
  assert (  (rz == rn));
  assert (  (rn == rz));
  assert (! (rz != rn));
  assert (! (rn != rz));
  assert (! (rz <  rn));
  assert (! (rn <  rz));
  assert (! (rz >  rn));
  assert (! (rn >  rz));
  assert (  (rz <= rn));
  assert (  (rn <= rz));
  assert (  (rz >= rn));
  assert (  (rn >= rz));
  
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

int main (void) 
{
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
  return 0;
}
