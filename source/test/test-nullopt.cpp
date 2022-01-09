/** test-nullopt.cpp
 * Copyright © 2022 Gene Harvey
 *
 * This software may be modified and distributed under the terms
 * of the MIT license. See the LICENSE file for details.
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "test_common.hpp"

int
main (void)
{
  gch::optional_ref<int> r0 = gch::nullopt;
  CHECK (! r0.has_value ());

  gch::optional_ref<int> r1 = { };
  CHECK (! r1.has_value ());

  gch::optional_ref<int> r2 (gch::nullopt);
  CHECK (! r2.has_value ());

  int x = 1;
  int y = 2;
  r0 = &x;
  CHECK (r0.has_value ());
  static_cast<void> (y);

  r1.emplace (x);
  CHECK (r1.has_value ());

  r2.emplace (x);
  CHECK (r2.has_value ());

  CHECK (r0.value () == x);
  CHECK (r0.value_or (y) == x);
  CHECK (r0.value_or (y) == 1);
  CHECK (r0.value_or (2) == 1);

  r0 = gch::nullopt;
  CHECK (! r0.has_value ());

  r1 = { };
  CHECK (! r1.has_value ());

  r2.reset ();
  CHECK (! r2.has_value ());

  CHECK (r0.value_or (y) == y);
  CHECK (r0.value_or (y) == 2);
  CHECK (r0.value_or (2) == 2);

  return 0;
}
