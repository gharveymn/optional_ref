/** test-throw.cpp
 * Copyright © 2022 Gene Harvey
 *
 * This software may be modified and distributed under the terms
 * of the MIT license. See the LICENSE file for details.
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "test_common.hpp"

#ifdef GCH_EXCEPTIONS

#include <iostream>
#include <stdexcept>

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
    std::cout << e.what () << std::endl;
    return 0;
  }
  catch (...)
  {
    std::cerr << "gch::optional_ref did not throw gch::bad_optional_access." << std::endl;
  }

  std::cerr << "gch::optional_ref did not throw at all." << std::endl;

  return 1;
}

#else

#include <csignal>

#if defined (__has_cpp_attribute) && __has_cpp_attribute (noreturn) >= 200809L
[[noreturn]]
#endif
static
void
bad_optional_access_handler (int signal)
{
  if (SIGABRT != signal)
  {
    std::fprintf (stderr, "Wrong signal recieved: %d.\n", signal);
    std::exit (EXIT_FAILURE);
  }

  std::printf ("SIGABRT correctly recieved.\n");
  std::exit (EXIT_SUCCESS);
}

int
main (void)
{
  std::signal (SIGABRT, bad_optional_access_handler);
  const gch::optional_ref<int> r;
  static_cast<void> (r.value ());
  return 1;
}

#endif
