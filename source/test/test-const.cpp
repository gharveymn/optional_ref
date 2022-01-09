/** test-const.cpp
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
  const int z = 1;

  gch::optional_ref<int> rx { x };
  gch::optional_ref<const int> rz;
  CHECK (rx.has_value ());
  CHECK (bool (rx));
  CHECK (! rz.has_value ());
  CHECK (! rz);
  CHECK (rx != rz);

  // set rz
  rz = &z;

  CHECK (rx == rz);
  CHECK (! rx.refers_to (z));
  CHECK (! rx.equal_pointer (&z));
  CHECK (! rx.equal_pointer (rz));
  CHECK (! equal_pointer (rx, rz));
  CHECK (rx.get_pointer () != rz.get_pointer ());

  // set rz with a non-const reference
  rz.emplace (y);

  CHECK (rx != rz);
  CHECK (rx.get_pointer () != rz.get_pointer ());

  rz.emplace (x);

  CHECK (rx == rz);
  CHECK (rx.get_pointer () == rz.get_pointer ());

  return 0;
}
