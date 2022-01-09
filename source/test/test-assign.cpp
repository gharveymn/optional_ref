/** test-assign.cpp
 * Copyright © 2022 Gene Harvey
 *
 * This software may be modified and distributed under the terms
 * of the MIT license. See the LICENSE file for details.
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "test_common.hpp"

int
main (void)
{
  int x = 1;
  gch::optional_ref<int> r { x };
  CHECK (r.refers_to (x));
  CHECK (r.equal_pointer (&x));
  CHECK (r == x);
  *r = 2;
  CHECK (r == x);

  int y = 3;
  CHECK (r != y);
  CHECK (r.emplace (y) == 3);
  CHECK (r.refers_to (y));
  CHECK (r.equal_pointer (&y));
  CHECK (r == y);
  static_cast<void> (y);

  r.reset ();
  CHECK (! r.refers_to (y));
  CHECK (! r.equal_pointer (&y));
  CHECK (r.equal_pointer (nullptr));
}
