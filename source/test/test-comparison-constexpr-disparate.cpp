/** test-comparison-constexpr-disparate.cpp
 * Copyright © 2022 Gene Harvey
 *
 * This software may be modified and distributed under the terms
 * of the MIT license. See the LICENSE file for details.
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "test_common.hpp"

int
main (void)
{
  static constexpr int  a[2] = { 11, 22 };
  static constexpr long b[2] = { 11, 22 };
  static constexpr gch::optional_ref<const int>  ra0 (a[0]);
  static constexpr gch::optional_ref<const long> rb0 (b[0]);
  static constexpr gch::optional_ref<const int>  ra1 (a[1]);
  static constexpr gch::optional_ref<const long> rb1 (b[1]);

  static constexpr gch::optional_ref<const gch::optional_ref<const int>> oo_a0 { ra0 };

  static_assert (oo_a0 == ra0,  "failed");
  static_assert (oo_a0 == rb0,  "failed");
  static_assert (oo_a0 != rb1,  "failed");
  static_assert (oo_a0 != ra1,  "failed");
  static_assert (oo_a0 == a[0], "failed");
  static_assert (oo_a0 == b[0], "failed");
  static_assert (oo_a0 != a[1], "failed");
  static_assert (oo_a0 != b[1], "failed");

  static constexpr gch::optional_ref<const gch::optional_ref<const int>> oo_a1 { ra1 };

  static_assert (oo_a1 != ra0,  "failed");
  static_assert (oo_a1 != rb0,  "failed");
  static_assert (oo_a1 == ra1,  "failed");
  static_assert (oo_a1 == rb1,  "failed");
  static_assert (oo_a1 != a[0], "failed");
  static_assert (oo_a1 != b[0], "failed");
  static_assert (oo_a1 == a[1], "failed");
  static_assert (oo_a1 == b[1], "failed");

  // not equal
  static_assert (! (ra0 == rb1), "failed");
  static_assert (! (rb1 == ra0), "failed");
  static_assert (  (ra0 != rb1), "failed");
  static_assert (  (rb1 != ra0), "failed");
  static_assert (  (ra0 <  rb1), "failed");
  static_assert (! (rb1 <  ra0), "failed");
  static_assert (! (ra0 >  rb1), "failed");
  static_assert (  (rb1 >  ra0), "failed");
  static_assert (  (ra0 <= rb1), "failed");
  static_assert (! (rb1 <= ra0), "failed");
  static_assert (! (ra0 >= rb1), "failed");
  static_assert (  (rb1 >= ra0), "failed");

  // gch::nullopt comparisons (rz is not gch::nullopt)
  constexpr gch::optional_ref<const int> rz (ra0);
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

  static_assert (  (rz == ra0), "failed");
  static_assert (  (ra0 == rz), "failed");
  static_assert (! (rz != ra0), "failed");
  static_assert (! (ra0 != rz), "failed");
  static_assert (! (rz <  ra0), "failed");
  static_assert (! (ra0 <  rz), "failed");
  static_assert (! (rz >  ra0), "failed");
  static_assert (! (ra0 >  rz), "failed");
  static_assert (  (rz <= ra0), "failed");
  static_assert (  (ra0 <= rz), "failed");
  static_assert (  (rz >= ra0), "failed");
  static_assert (  (ra0 >= rz), "failed");

  // set gch::nullopt
  constexpr gch::optional_ref<int> rn = { };

  // same as above, except rn contains gch::nullopt this time
  static_assert (! (ra0 == rn), "failed");
  static_assert (! (rn == ra0), "failed");
  static_assert (  (ra0 != rn), "failed");
  static_assert (  (rn != ra0), "failed");
  static_assert (! (ra0 <  rn), "failed");
  static_assert (  (rn <  ra0), "failed");
  static_assert (  (ra0 >  rn), "failed");
  static_assert (! (rn >  ra0), "failed");
  static_assert (! (ra0 <= rn), "failed");
  static_assert (  (rn <= ra0), "failed");
  static_assert (  (ra0 >= rn), "failed");
  static_assert (! (rn >= ra0), "failed");

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
  static constexpr const int& py = b[1];
  static_assert (! (ra0 == py ), "failed");
  static_assert (! (py  == ra0), "failed");
  static_assert (  (ra0 != py ), "failed");
  static_assert (  (py  != ra0), "failed");
  static_assert (  (ra0 <  py ), "failed");
  static_assert (! (py  <  ra0), "failed");
  static_assert (! (ra0 >  py ), "failed");
  static_assert (  (py  >  ra0), "failed");
  static_assert (  (ra0 <= py ), "failed");
  static_assert (! (py  <= ra0), "failed");
  static_assert (! (ra0 >= py ), "failed");
  static_assert (  (py  >= ra0), "failed");

  // compare with a reference (equal)
  static constexpr const int& px = b[0];
  static_assert (  (ra0 == px ), "failed");
  static_assert (  (px  == ra0), "failed");
  static_assert (! (ra0 != px ), "failed");
  static_assert (! (px  != ra0), "failed");
  static_assert (! (ra0 <  px ), "failed");
  static_assert (! (px  <  ra0), "failed");
  static_assert (! (ra0 >  px ), "failed");
  static_assert (! (px  >  ra0), "failed");
  static_assert (  (ra0 <= px ), "failed");
  static_assert (  (px  <= ra0), "failed");
  static_assert (  (ra0 >= px ), "failed");
  static_assert (  (px  >= ra0), "failed");

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
