/** test_common.hpp
 * Copyright © 2022 Gene Harvey
 *
 * This software may be modified and distributed under the terms
 * of the MIT license. See the LICENSE file for details.
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef OPTIONAL_REF_TEST_COMMON_HPP
#define OPTIONAL_REF_TEST_COMMON_HPP

#include "gch/optional_ref.hpp"

#include <cstdio>

#define CHECK(EXPR)                                                           \
if (! (EXPR))                                                                 \
{                                                                             \
  printf ("Check failed in file " __FILE__ " at line %i:\n" #EXPR, __LINE__); \
  return 1;                                                                   \
} (void)0

#endif // OPTIONAL_REF_TEST_COMMON_HPP
