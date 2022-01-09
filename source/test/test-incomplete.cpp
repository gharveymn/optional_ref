/** test-incomplete.cpp
 * Copyright © 2022 Gene Harvey
 *
 * This software may be modified and distributed under the terms
 * of the MIT license. See the LICENSE file for details.
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "test_common.hpp"

struct incomp_struct;
incomp_struct& get_incomp (void);

int
main (void)
{
  incomp_struct& incomp = get_incomp ();
  gch::optional_ref<incomp_struct> o (incomp);
  gch::optional_ref<incomp_struct> op (&incomp);
  gch::optional_ref<incomp_struct> oc (o);

  CHECK (o.equal_pointer (op));
  CHECK (equal_pointer (o, op, oc));

  return 0;
}

struct incomp_struct { };
incomp_struct& get_incomp (void)
{
  static incomp_struct incomp { };
  return incomp;
}
