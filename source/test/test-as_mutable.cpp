/** test-as_mutable.cpp
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
  gch::optional_ref<const int> r { x };
  auto rm = as_mutable (r);
  static_assert (std::is_same<decltype (rm), gch::optional_ref<int>>::value, "");

  auto rmm = as_mutable (rm);
  static_assert (std::is_same<decltype (rmm), gch::optional_ref<int>>::value, "");
}
