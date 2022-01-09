/** test-throw.cpp
 * Copyright © 2022 Gene Harvey
 *
 * This software may be modified and distributed under the terms
 * of the MIT license. See the LICENSE file for details.
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "test_common.hpp"

#include <stdexcept>

int
main (void)
{
  const gch::optional_ref<int> r;

  try
  {
    static_cast<void> (r.value ());
  }
  catch (const gch::bad_optional_access& e) { static_cast<void> (e.what ()); }
  catch (...) { throw std::runtime_error ("optional_ref does not throw bad_optional_access"); }
}
