/** test-throw.cpp
 * Copyright © 2022 Gene Harvey
 *
 * This software may be modified and distributed under the terms
 * of the MIT license. See the LICENSE file for details.
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "test_common.hpp"

#include <stdexcept>
#include <iostream>

int
main (void)
{
  const gch::optional_ref<int> r;

  try
  {
    static_cast<void> (r.value ());
  }
  catch (const gch::bad_optional_access& e)
  {
    static_cast<void> (e.what ());
    return 0;
  }
  catch (...)
  {
    std::cerr << "gch::optional_ref did not throw gch::bad_optional_access." << std::endl;
  }

  std::cerr << "gch::optional_ref did not throw at all." << std::endl;

  return 1;
}
