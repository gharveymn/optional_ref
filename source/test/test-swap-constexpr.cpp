/** test-swap-constexpr.cpp
 * Copyright © 2022 Gene Harvey
 *
 * This software may be modified and distributed under the terms
 * of the MIT license. See the LICENSE file for details.
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "test_common.hpp"

static constexpr
bool
test (void)
{
#ifdef GCH_HAS_CPP14_CONSTEXPR
  int x = 1;
  int y = 2;
  gch::optional_ref<int> rx { x };
  gch::optional_ref<int> ry { y };
  gch::optional_ref<int> rz { x };

  bool t1 = (rx != ry) && (rx == rz);

  using std::swap;
  swap (ry, rz);

  bool t2 = (rx == ry) && (rx != rz);

  gch::optional_ref<int> rm { x };
  gch::optional_ref<int> rn { gch::nullopt };

  bool t3 = (rn == gch::nullopt) && (rm != gch::nullopt) && (rm != rn) && (rm == rx);

  using std::swap;
  swap (rm, rn);

  bool t4 = (rn != gch::nullopt) && (rm == gch::nullopt) && (rm != rn) && (rn == rx);

  return t1 && t2 && t3 && t4;
#endif

  return true;
}

static_assert (test (), "failed swap");

int
main (void)
{
  return 0;
}
