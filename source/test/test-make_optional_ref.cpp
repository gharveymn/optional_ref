/** test-make_optional_ref.cpp
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
  const int y = 2;
  gch::optional_ref<int> rx = gch::make_optional_ref (x);
  gch::optional_ref<const int> ry = gch::make_optional_ref (y);
  CHECK (rx != ry);

  return 0;
}
