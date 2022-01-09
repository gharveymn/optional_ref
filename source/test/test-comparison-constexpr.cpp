/** test-comparison-constexpr.cpp
 * Copyright © 2022 Gene Harvey
 *
 * This software may be modified and distributed under the terms
 * of the MIT license. See the LICENSE file for details.
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "test_common.hpp"

int
main (void)
{
  static constexpr int a[2] = { 11, 22 };
  static constexpr gch::optional_ref<const int> rx (a[0]);
  static constexpr gch::optional_ref<const int> ry (a[1]);

  static constexpr gch::optional_ref<const gch::optional_ref<const int>> oo { ry };

  static_assert (oo == ry,   " ");
  static_assert (oo == a[1], " ");

  // not equal
  static_assert (! (rx == ry), "failed");
  static_assert (! (ry == rx), "failed");
  static_assert (  (rx != ry), "failed");
  static_assert (  (ry != rx), "failed");
  static_assert (  (rx <  ry), "failed");
  static_assert (! (ry <  rx), "failed");
  static_assert (! (rx >  ry), "failed");
  static_assert (  (ry >  rx), "failed");
  static_assert (  (rx <= ry), "failed");
  static_assert (! (ry <= rx), "failed");
  static_assert (! (rx >= ry), "failed");
  static_assert (  (ry >= rx), "failed");

  // gch::nullopt comparisons (rz is not gch::nullopt)
  constexpr gch::optional_ref<const int> rz (rx);
  static_assert (! (rz      == gch::nullopt), "failed");
  static_assert (! (gch::nullopt == rz     ), "failed");
  static_assert (  (rz      != gch::nullopt), "failed");
  static_assert (  (gch::nullopt != rz     ), "failed");
  static_assert (! (rz      <  gch::nullopt), "failed");
  static_assert (  (gch::nullopt <  rz     ), "failed");
  static_assert (  (rz      >  gch::nullopt), "failed");
  static_assert (! (gch::nullopt >  rz     ), "failed");
  static_assert (! (rz      <= gch::nullopt), "failed");
  static_assert (  (gch::nullopt <= rz     ), "failed");
  static_assert (  (rz      >= gch::nullopt), "failed");
  static_assert (! (gch::nullopt >= rz     ), "failed");

  static_assert (  (rz == rx), "failed");
  static_assert (  (rx == rz), "failed");
  static_assert (! (rz != rx), "failed");
  static_assert (! (rx != rz), "failed");
  static_assert (! (rz <  rx), "failed");
  static_assert (! (rx <  rz), "failed");
  static_assert (! (rz >  rx), "failed");
  static_assert (! (rx >  rz), "failed");
  static_assert (  (rz <= rx), "failed");
  static_assert (  (rx <= rz), "failed");
  static_assert (  (rz >= rx), "failed");
  static_assert (  (rx >= rz), "failed");

  // set gch::nullopt
  constexpr gch::optional_ref<int> rn = { };

  // same as above, except rn contains gch::nullopt this time
  static_assert (! (rx == rn), "failed");
  static_assert (! (rn == rx), "failed");
  static_assert (  (rx != rn), "failed");
  static_assert (  (rn != rx), "failed");
  static_assert (! (rx <  rn), "failed");
  static_assert (  (rn <  rx), "failed");
  static_assert (  (rx >  rn), "failed");
  static_assert (! (rn >  rx), "failed");
  static_assert (! (rx <= rn), "failed");
  static_assert (  (rn <= rx), "failed");
  static_assert (  (rx >= rn), "failed");
  static_assert (! (rn >= rx), "failed");

  // rn is still gch::nullopt
  static_assert (  (rn      == gch::nullopt), "failed");
  static_assert (  (gch::nullopt == rn     ), "failed");
  static_assert (! (rn      != gch::nullopt), "failed");
  static_assert (! (gch::nullopt != rn     ), "failed");
  static_assert (! (rn      <  gch::nullopt), "failed");
  static_assert (! (gch::nullopt <  rn     ), "failed");
  static_assert (! (rn      >  gch::nullopt), "failed");
  static_assert (! (gch::nullopt >  rn     ), "failed");
  static_assert (  (rn      <= gch::nullopt), "failed");
  static_assert (  (gch::nullopt <= rn     ), "failed");
  static_assert (  (rn      >= gch::nullopt), "failed");
  static_assert (  (gch::nullopt >= rn     ), "failed");

  // compare two optional_refs which are both gch::nullopt
  constexpr gch::optional_ref<int> rm (gch::nullopt);
  static_assert (  (rn == rm), "failed");
  static_assert (  (rm == rn), "failed");
  static_assert (! (rn != rm), "failed");
  static_assert (! (rm != rn), "failed");
  static_assert (! (rn <  rm), "failed");
  static_assert (! (rm <  rn), "failed");
  static_assert (! (rn >  rm), "failed");
  static_assert (! (rm >  rn), "failed");
  static_assert (  (rn <= rm), "failed");
  static_assert (  (rm <= rn), "failed");
  static_assert (  (rn >= rm), "failed");
  static_assert (  (rm >= rn), "failed");

  // compare with a reference (not equal)
  static constexpr const int& py = a[1];
  static_assert (! (rx == py), "failed");
  static_assert (! (py == rx), "failed");
  static_assert (  (rx != py), "failed");
  static_assert (  (py != rx), "failed");
  static_assert (  (rx <  py), "failed");
  static_assert (! (py <  rx), "failed");
  static_assert (! (rx >  py), "failed");
  static_assert (  (py >  rx), "failed");
  static_assert (  (rx <= py), "failed");
  static_assert (! (py <= rx), "failed");
  static_assert (! (rx >= py), "failed");
  static_assert (  (py >= rx), "failed");

  // compare with a pointer (equal)
  static constexpr const int& px = a[0];
  static_assert (  (rx == px), "failed");
  static_assert (  (px == rx), "failed");
  static_assert (! (rx != px), "failed");
  static_assert (! (px != rx), "failed");
  static_assert (! (rx <  px), "failed");
  static_assert (! (px <  rx), "failed");
  static_assert (! (rx >  px), "failed");
  static_assert (! (px >  rx), "failed");
  static_assert (  (rx <= px), "failed");
  static_assert (  (px <= rx), "failed");
  static_assert (  (rx >= px), "failed");
  static_assert (  (px >= rx), "failed");

  // compare pointer with gch::optional_ref which is gch::nullopt
  static_assert (! (rn == py), "failed");
  static_assert (! (py == rn), "failed");
  static_assert (  (rn != py), "failed");
  static_assert (  (py != rn), "failed");
  static_assert (  (rn <  py), "failed");
  static_assert (! (py <  rn), "failed");
  static_assert (! (rn >  py), "failed");
  static_assert (  (py >  rn), "failed");
  static_assert (  (rn <= py), "failed");
  static_assert (! (py <= rn), "failed");
  static_assert (! (rn >= py), "failed");
  static_assert (  (py >= rn), "failed");

  return 0;
}
