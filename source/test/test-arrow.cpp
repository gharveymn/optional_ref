/** test-arrow.cpp
 * Copyright © 2022 Gene Harvey
 *
 * This software may be modified and distributed under the terms
 * of the MIT license. See the LICENSE file for details.
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "test_common.hpp"

int
main (void)
{
  struct my_struct
  {
    int x;
  } s { 1 };

  const gch::optional_ref<my_struct> r { s };
  CHECK (r->x == 1);

  return 0;
}
