/** test-inheritence.cpp
 * Copyright © 2022 Gene Harvey
 *
 * This software may be modified and distributed under the terms
 * of the MIT license. See the LICENSE file for details.
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "test_common.hpp"

struct my_struct_base
{
  int x;
};

constexpr
bool
operator== (const my_struct_base& lhs, const my_struct_base& rhs) noexcept
{
return lhs.x == rhs.x;
}

constexpr
bool
operator!= (const my_struct_base& lhs, const my_struct_base& rhs) noexcept
{
return ! (lhs == rhs);
}

struct my_struct : my_struct_base
{ };

int
main (void)
{
  my_struct s0 { };
  my_struct s1 { };
  s1.x = 1;
  const gch::optional_ref<my_struct_base> r0 = &s0;
  const gch::optional_ref<my_struct> r1 (s1);

  CHECK (r0 != r1);
  CHECK (r0.get_pointer () != r1.get_pointer ());

  const gch::optional_ref<my_struct_base> r2 (r1);

  CHECK (r2 == r1);
  CHECK (r2.get_pointer () == r1.get_pointer ());

  return 0;
}
