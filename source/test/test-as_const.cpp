/** test-as_const.cpp
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
  auto rc = as_const (r);
  static_assert (std::is_same<decltype (rc), gch::optional_ref<const int>>::value, "");

  auto rcc = as_const (rc);
  static_assert (std::is_same<decltype (rcc), gch::optional_ref<const int>>::value, "");

  return 0;
}
