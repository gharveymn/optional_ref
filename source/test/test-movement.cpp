/** test-movement.cpp
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
  int y = 2;
  const gch::optional_ref<int> rx { x };
  const gch::optional_ref<int> ry { y };
  CHECK (! (rx == ry));
  CHECK (  (rx != ry));

  // copy constructor
  gch::optional_ref<int> rz { rx };
  CHECK (rz == rx);
  CHECK (rz != ry);
  CHECK (rz.get_pointer () == rx.get_pointer ());
  CHECK (rz.get_pointer () != ry.get_pointer ());

  // move constructor
  gch::optional_ref<int> rm { std::move (rz) };
  CHECK (rm == rx);
  CHECK (rm != ry);
  CHECK (rm.get_pointer () == rx.get_pointer ());
  CHECK (rm.get_pointer () != ry.get_pointer ());

  // copy assignment operator
  rz = ry;
  CHECK (rz != rx);
  CHECK (rz == ry);
  CHECK (rz.get_pointer () != rx.get_pointer ());
  CHECK (rz.get_pointer () == ry.get_pointer ());

  // move assignment operator
  rm = std::move (rz);
  CHECK (rm != rx);
  CHECK (rm == ry);
  CHECK (rm.get_pointer () != rx.get_pointer ());
  CHECK (rm.get_pointer () == ry.get_pointer ());

  // creation using a temporary
  const gch::optional_ref<int> rt { gch::optional_ref<int> { x } };
  CHECK (rt == x);
  CHECK (rt != y);

  // swap

  gch::optional_ref<int> rp { x };
  gch::optional_ref<int> rq { y };

  using std::swap;
  swap (rp, rq);

  CHECK (! (rp == rq));
  CHECK (  (rp != rq));

  return 0;
}
