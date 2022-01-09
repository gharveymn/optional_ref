/** test-comparison.cpp
 * Copyright © 2022 Gene Harvey
 *
 * This software may be modified and distributed under the terms
 * of the MIT license. See the LICENSE file for details.
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "test_common.hpp"

int
main (void)
{
  int a[2] = { 11, 22 };
  int b[2] = { 11, 22 };
  const gch::optional_ref<int> rx (a[0]);
  const gch::optional_ref<int> ry (a[1]);

  // not equal
  CHECK (! (rx == ry));
  CHECK (! (ry == rx));
  CHECK (  (rx != ry));
  CHECK (  (ry != rx));
  CHECK (  (rx <  ry));
  CHECK (! (ry <  rx));
  CHECK (! (rx >  ry));
  CHECK (  (ry >  rx));
  CHECK (  (rx <= ry));
  CHECK (! (ry <= rx));
  CHECK (! (rx >= ry));
  CHECK (  (ry >= rx));

  // set value to be equal (should be equal in comparison)
  *ry = 11;
  CHECK (  (rx == ry));
  CHECK (  (ry == rx));
  CHECK (! (rx != ry));
  CHECK (! (ry != rx));
  CHECK (! (rx <  ry));
  CHECK (! (ry <  rx));
  CHECK (! (rx >  ry));
  CHECK (! (ry >  rx));
  CHECK (  (rx <= ry));
  CHECK (  (ry <= rx));
  CHECK (  (rx >= ry));
  CHECK (  (ry >= rx));

  // gch::nullopt comparisons (rz is not gch::nullopt)
  const gch::optional_ref<int> rz (rx);
  CHECK (! (rz      == gch::nullopt));
  CHECK (! (gch::nullopt == rz     ));
  CHECK (  (rz      != gch::nullopt));
  CHECK (  (gch::nullopt != rz     ));
  CHECK (! (rz      <  gch::nullopt));
  CHECK (  (gch::nullopt <  rz     ));
  CHECK (  (rz      >  gch::nullopt));
  CHECK (! (gch::nullopt >  rz     ));
  CHECK (! (rz      <= gch::nullopt));
  CHECK (  (gch::nullopt <= rz     ));
  CHECK (  (rz      >= gch::nullopt));
  CHECK (! (gch::nullopt >= rz     ));

  // set gch::nullopt
  const gch::optional_ref<int> rn = { };

  // same as above, except rn contains gch::nullopt this time
  CHECK (! (rx == rn));
  CHECK (! (rn == rx));
  CHECK (  (rx != rn));
  CHECK (  (rn != rx));
  CHECK (! (rx <  rn));
  CHECK (  (rn <  rx));
  CHECK (  (rx >  rn));
  CHECK (! (rn >  rx));
  CHECK (! (rx <= rn));
  CHECK (  (rn <= rx));
  CHECK (  (rx >= rn));
  CHECK (! (rn >= rx));

  // rn is still gch::nullopt
  CHECK (  (rn      == gch::nullopt));
  CHECK (  (gch::nullopt == rn     ));
  CHECK (! (rn      != gch::nullopt));
  CHECK (! (gch::nullopt != rn     ));
  CHECK (! (rn      <  gch::nullopt));
  CHECK (! (gch::nullopt <  rn     ));
  CHECK (! (rn      >  gch::nullopt));
  CHECK (! (gch::nullopt >  rn     ));
  CHECK (  (rn      <= gch::nullopt));
  CHECK (  (gch::nullopt <= rn     ));
  CHECK (  (rn      >= gch::nullopt));
  CHECK (  (gch::nullopt >= rn     ));

  // compare two optional_refs which are both gch::nullopt
  const gch::optional_ref<int> rm (gch::nullopt);
  CHECK (  (rn == rm));
  CHECK (  (rm == rn));
  CHECK (! (rn != rm));
  CHECK (! (rm != rn));
  CHECK (! (rn <  rm));
  CHECK (! (rm <  rn));
  CHECK (! (rn >  rm));
  CHECK (! (rm >  rn));
  CHECK (  (rn <= rm));
  CHECK (  (rm <= rn));
  CHECK (  (rn >= rm));
  CHECK (  (rm >= rn));

  // compare with a reference (not equal)
  int& py = b[1];
  CHECK (! (rx == py));
  CHECK (! (py == rx));
  CHECK (  (rx != py));
  CHECK (  (py != rx));
  CHECK (  (rx <  py));
  CHECK (! (py <  rx));
  CHECK (! (rx >  py));
  CHECK (  (py >  rx));
  CHECK (  (rx <= py));
  CHECK (! (py <= rx));
  CHECK (! (rx >= py));
  CHECK (  (py >= rx));
  static_cast<void> (py);

  // compare with a reference (equal)
  int& px = b[0];
  CHECK (  (rx == px));
  CHECK (  (px == rx));
  CHECK (! (rx != px));
  CHECK (! (px != rx));
  CHECK (! (rx <  px));
  CHECK (! (px <  rx));
  CHECK (! (rx >  px));
  CHECK (! (px >  rx));
  CHECK (  (rx <= px));
  CHECK (  (px <= rx));
  CHECK (  (rx >= px));
  CHECK (  (px >= rx));
  static_cast<void> (px);

  // compare pointer with optional_ref which is gch::nullopt
  CHECK (! (rn == py));
  CHECK (! (py == rn));
  CHECK (  (rn != py));
  CHECK (  (py != rn));
  CHECK (  (rn <  py));
  CHECK (! (py <  rn));
  CHECK (! (rn >  py));
  CHECK (  (py >  rn));
  CHECK (  (rn <= py));
  CHECK (! (py <= rn));
  CHECK (! (rn >= py));
  CHECK (  (py >= rn));

  return 0;
}
