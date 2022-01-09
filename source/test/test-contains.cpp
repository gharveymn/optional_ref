/** test-contains.cpp
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

  gch::optional_ref<int> rx (x);
  gch::optional_ref<int> ry (y);

  gch::optional_ref<int> rx1 (x);

  CHECK (  rx.refers_to (x));
  CHECK (! rx.refers_to (y));

  CHECK (! rx.refers_to (*ry));
  CHECK (  rx.refers_to (*rx1));

  return 0;
}
