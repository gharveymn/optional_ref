/** test-instantiation.cpp
 * Copyright © 2022 Gene Harvey
 *
 * This software may be modified and distributed under the terms
 * of the MIT license. See the LICENSE file for details.
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "gch/optional_ref.hpp"

struct base { };
struct derived : base { };

// these cause an ICE with MSVC for some reason
#ifndef _MSC_VER

template class gch::optional_ref<int>;

template class gch::optional_ref<base>;
template class gch::optional_ref<derived>;
template gch::optional_ref<base>::optional_ref (base&);
template gch::optional_ref<base>::optional_ref (derived&);
template gch::optional_ref<base>::optional_ref (optional_ref<derived>) noexcept;
template auto gch::optional_ref<base>::value_or (base&) const -> reference;
template auto gch::optional_ref<base>::value_or (derived&) const -> reference;
template auto gch::optional_ref<base>::emplace (base&) -> reference;
template auto gch::optional_ref<base>::emplace (derived&) -> reference;
template bool gch::optional_ref<base>::refers_to (base&) const noexcept;
template bool gch::optional_ref<base>::refers_to (derived&) const noexcept;
template bool gch::optional_ref<base>::equal_pointer (optional_ref<base>) const noexcept;
template bool gch::optional_ref<base>::equal_pointer (optional_ref<derived>) const noexcept;

#endif

template bool gch::operator== (optional_ref<int>, optional_ref<int>);
template bool gch::operator!= (optional_ref<int>, optional_ref<int>);
template bool gch::operator<  (optional_ref<int>, optional_ref<int>);
template bool gch::operator>  (optional_ref<int>, optional_ref<int>);
template bool gch::operator<= (optional_ref<int>, optional_ref<int>);
template bool gch::operator>= (optional_ref<int>, optional_ref<int>);
#ifdef GCH_LIB_THREE_WAY_COMPARISON
template std::strong_ordering gch::operator<=> (optional_ref<int>, optional_ref<int>);
#endif

template bool gch::operator== (optional_ref<int>, nullopt_t) noexcept;
#ifdef GCH_LIB_THREE_WAY_COMPARISON
template std::strong_ordering gch::operator<=> (optional_ref<int>, nullopt_t) noexcept;
#else
template bool gch::operator== (nullopt_t        , optional_ref<int>) noexcept;
template bool gch::operator!= (optional_ref<int>, nullopt_t        ) noexcept;
template bool gch::operator!= (nullopt_t        , optional_ref<int>) noexcept;
template bool gch::operator<  (optional_ref<int>, nullopt_t        ) noexcept;
template bool gch::operator<  (nullopt_t        , optional_ref<int>) noexcept;
template bool gch::operator>  (optional_ref<int>, nullopt_t        ) noexcept;
template bool gch::operator>  (nullopt_t        , optional_ref<int>) noexcept;
template bool gch::operator<= (optional_ref<int>, nullopt_t        ) noexcept;
template bool gch::operator<= (nullopt_t        , optional_ref<int>) noexcept;
template bool gch::operator>= (optional_ref<int>, nullopt_t        ) noexcept;
template bool gch::operator>= (nullopt_t        , optional_ref<int>) noexcept;
#endif

template bool gch::operator== (optional_ref<int>, const int&       ) noexcept;
template bool gch::operator== (const int&       , optional_ref<int>) noexcept;
template bool gch::operator!= (optional_ref<int>, const int&       ) noexcept;
template bool gch::operator!= (const int&       , optional_ref<int>) noexcept;
template bool gch::operator<  (optional_ref<int>, const int&       );
template bool gch::operator<  (const int&       , optional_ref<int>);
template bool gch::operator>  (optional_ref<int>, const int&       );
template bool gch::operator>  (const int&       , optional_ref<int>);
template bool gch::operator<= (optional_ref<int>, const int&       );
template bool gch::operator<= (const int&       , optional_ref<int>);
template bool gch::operator>= (optional_ref<int>, const int&       );
template bool gch::operator>= (const int&       , optional_ref<int>);
#ifdef GCH_LIB_THREE_WAY_COMPARISON
template std::strong_ordering gch::operator<=> (optional_ref<int>, const int&);
#endif

static_assert (std::is_trivially_copy_constructible<gch::optional_ref<int>>::value, "");
static_assert (std::is_trivially_move_constructible<gch::optional_ref<int>>::value, "");
static_assert (std::is_trivially_copy_assignable<gch::optional_ref<int>>::value, "");
static_assert (std::is_trivially_move_assignable<gch::optional_ref<int>>::value, "");
static_assert (std::is_trivially_destructible<gch::optional_ref<int>>::value, "");
static_assert (std::is_trivially_copyable<gch::optional_ref<int>>::value, "");

static constexpr int g_x = 0;
static constexpr gch::optional_ref<const int> g_rx { g_x };

int
main (void)
{
  static_cast<void> (g_rx);

  return 0;
}
