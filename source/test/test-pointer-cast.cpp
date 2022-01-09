/** test-pointer-cast.cpp
 * Copyright © 2022 Gene Harvey
 *
 * This software may be modified and distributed under the terms
 * of the MIT license. See the LICENSE file for details.
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "test_common.hpp"

int
main (void)
{
  int  x     = 1;
  int* x_ptr = &x;
  int* n_ptr = nullptr;

  const gch::optional_ref<int> opt_x = x_ptr;
  CHECK (opt_x.has_value ());
  CHECK (opt_x.refers_to (x));

  const gch::optional_ref<int> opt_n = n_ptr;
  CHECK (! opt_n.has_value ());

  /* implicitly convertible */
  struct imp_conv
  {
    /* implicit */ operator int* (void) noexcept { return m_ptr; }
    int* m_ptr;
  } imp { &x };

  const gch::optional_ref<int> opt_y = imp;
  CHECK (opt_y.has_value ());
  CHECK (opt_y.refers_to (x));

  /* explicitly convertible */
  struct exp_conv
  {
    explicit operator int* (void) noexcept { return m_ptr; }
    int* m_ptr;
  } exp { &x };

  const gch::optional_ref<int> opt_z = gch::optional_ref<int> { exp };
  CHECK (opt_z.has_value ());
  CHECK (opt_z.refers_to (x));

  return 0;
}
