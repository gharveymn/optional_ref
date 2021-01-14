/** optional_ref.hpp
 * Defines an optional reference.
 *
 * Copyright © 2019 Gene Harvey
 *
 * This software may be modified and distributed under the terms
 * of the MIT license. See the LICENSE file for details.
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef OPTIONAL_REF_HPP
#define OPTIONAL_REF_HPP

#include <exception>
#include <functional>
#include <type_traits>
#include <utility>

#ifdef __cpp_constexpr
#  ifndef GCH_CPP14_CONSTEXPR
#    if __cpp_constexpr >= 201304L
#      define GCH_CPP14_CONSTEXPR constexpr
#      ifndef GCH_HAS_CPP14_CONSTEXPR
#        define GCH_HAS_CPP14_CONSTEXPR
#      endif
#    else
#      define GCH_CPP14_CONSTEXPR
#    endif
#  endif
#  ifndef GCH_CPP17_CONSTEXPR
#    if __cpp_constexpr >= 201603L
#      define GCH_CPP17_CONSTEXPR constexpr
#      ifndef GCH_HAS_CPP17_CONSTEXPR
#        define GCH_HAS_CPP17_CONSTEXPR
#      endif
#    else
#      define GCH_CPP17_CONSTEXPR
#    endif
#  endif
#  ifndef GCH_CPP20_CONSTEXPR
#    if __cpp_constexpr >= 201907L
#      define GCH_CPP20_CONSTEXPR constexpr
#      ifndef GCH_HAS_CPP20_CONSTEXPR
#        define GCH_HAS_CPP20_CONSTEXPR
#      endif
#    else
#      define GCH_CPP20_CONSTEXPR
#    endif
#  endif
#endif

#ifndef GCH_NODISCARD
#  if defined (__has_cpp_attribute) && __has_cpp_attribute (nodiscard) >= 201603L
#    if ! defined (__clang__) || defined (GCH_CLANG_17)
#      define GCH_NODISCARD [[nodiscard]]
#    else
#      define GCH_NODISCARD
#    endif
#  else
#    define GCH_NODISCARD
#  endif
#endif

#ifndef GCH_SWAP_CONSTEXPR
#  if defined (__cpp_lib_constexpr_algorithms) && __cpp_lib_constexpr_algorithms >= 201806L
#    define GCH_SWAP_CONSTEXPR constexpr
#  else
#    define GCH_SWAP_CONSTEXPR
#  endif
#endif

#ifndef GCH_INLINE_VAR
#  if defined (__cpp_inline_variables) && __cpp_inline_variables >= 201606
#    define GCH_INLINE_VAR inline
#  else
#    define GCH_INLINE_VAR
#  endif
#endif

#if defined (__cpp_deduction_guides) && __cpp_deduction_guides >= 201703
#  ifndef GCH_CTAD_SUPPORT
#    define GCH_CTAD_SUPPORT
#  endif
#endif

#if defined (__cpp_impl_three_way_comparison) && __cpp_impl_three_way_comparison >= 201907L
#  ifndef GCH_IMPL_THREE_WAY_COMPARISON
#    define GCH_IMPL_THREE_WAY_COMPARISON
#  endif
#  if __has_include(<compare>)
#    include <compare>
#    if defined (__cpp_lib_three_way_comparison) && __cpp_lib_three_way_comparison >= 201907L
#      ifndef GCH_LIB_THREE_WAY_COMPARISON
#        define GCH_LIB_THREE_WAY_COMPARISON
#      endif
#    endif
#  endif
#endif

namespace gch
{

  /**
   * A struct that provides semantics for
   * empty `optional_ref`s.
   */
  struct nullopt_t
  {
    static constexpr struct create_tag { } create { };
    constexpr explicit nullopt_t (create_tag) noexcept { }
  };

  GCH_INLINE_VAR constexpr nullopt_t nullopt { nullopt_t::create };

  /**
   * An exception class for cases of bad access
   * to an `optional_ref`.
   */
  class bad_optional_access : public std::exception
  {
  public:
    bad_optional_access            (void)                           = default;
    bad_optional_access            (const bad_optional_access&)     = default;
    bad_optional_access            (bad_optional_access&&) noexcept = default;
    bad_optional_access& operator= (const bad_optional_access&)     = default;
    bad_optional_access& operator= (bad_optional_access&&) noexcept = default;
    ~bad_optional_access           (void) noexcept override         = default;

    GCH_NODISCARD
    const char*
    what (void) const noexcept override { return "bad optional_ref access"; }
  };

  /**
   * A reference wrapper which provides semantics similar to `std::optional`.
   *
   * @tparam Value the value type of the stored reference.
   */
  template <typename Value>
  class optional_ref
  {
  public:
    static_assert(! std::is_reference<Value>::value,
      "optional_ref expects a value type as a template argument, not a reference.");

    using value_type      = Value;        /*!< The value type of the stored reference */
    using reference       = Value&;       /*!< The reference type to be wrapped       */
    using pointer         = Value*;       /*!< The pointer type to the value type     */
    using const_reference = const Value&; /*!< A constant reference to `Value`        */
    using const_pointer   = const Value*; /*!< A constant pointer to `Value`          */

  private:
    template <typename U>
    using constructible_from_pointer_to =
      std::is_constructible<pointer, decltype (&std::declval<U&> ())>;

    template <typename U>
    using pointer_to_is_convertible =
      std::is_convertible<decltype (&std::declval<U&> ()), pointer>;

    template <typename T> struct is_optional_ref                  : std::false_type { };
    template <typename T> struct is_optional_ref<optional_ref<T>> : std::true_type  { };

  public:
    //! For the record, I don't think the explicit conversions are even possible
    //! since we are only using raw pointers, but I can't find a definitive
    //! reference to that fact, so they stay.

    optional_ref            (void)                    = default;
    optional_ref            (const optional_ref&)     = default;
    optional_ref            (optional_ref&&) noexcept = default;
    optional_ref& operator= (const optional_ref&)     = default;
    optional_ref& operator= (optional_ref&&) noexcept = default;
    ~optional_ref           (void)                    = default;

    /**
     * Constructor
     *
     * A constructor to implicitly convert nullopt_t so we can
     * use syntax like `optional_ref<int> x = nullopt`.
     */
    constexpr /* implicit */
    optional_ref (nullopt_t) noexcept
    { };

    /**
     * Constructor
     *
     * A constructor for the case where `U*` is
     * implicitly convertible to type `pointer`.
     *
     * @tparam U a referenced value type.
     * @param ref a reference whose pointer is implicitly convertible to type `pointer`.
     */
    template <typename U,
              typename std::enable_if<constructible_from_pointer_to<U>::value
                                  &&  pointer_to_is_convertible<U>::value>::type * = nullptr>
    constexpr /* implicit */
    optional_ref (U& ref) noexcept
      : m_ptr (&ref)
    { }

    /**
     * Constructor
     *
     * A constructor for the case where `pointer` is
     * explicitly constructable from `U*`.
     *
     * @tparam U a referenced value type.
     * @param ref a argument from which `pointer` may be explicitly constructed.
     */
    template <typename U,
              typename std::enable_if<constructible_from_pointer_to<U>::value
                                  &&! pointer_to_is_convertible<U>::value>::type * = nullptr>
    constexpr explicit
    optional_ref (U& ref) noexcept
      : m_ptr (static_cast<pointer> (&ref))
    { }

    /**
     * Constructor
     *
     * A deleted contructor for the case where `ref` is an rvalue reference.
     */
    template <typename U,
              typename = typename std::enable_if<constructible_from_pointer_to<U>::value>::type>
    optional_ref (const U&&) = delete;

    /**
     * Constructor
     *
     * A converting constructor for types implicitly
     * convertible to `pointer`. This is so we don't
     * need to dereference or inspect pointers before
     * creation; we can just use the pointer directly.
     *
     * @tparam U a type implicitly convertible to `pointer`.
     * @param ptr a pointer.
     */
    template <typename U,
              typename std::enable_if<std::is_constructible<pointer, U>::value
                                  &&  std::is_convertible<U, pointer>::value>::type * = nullptr>
    constexpr /* implicit */
    optional_ref (U&& ptr) noexcept
      : m_ptr (ptr)
    { }

    /**
     * Constructor
     *
     * A converting constructor for types explicitly
     * convertible to `pointer`. This is so we don't
     * need to dereference or inspect pointers before
     * creation; we can just use the pointer directly.
     *
     * @tparam U a type explicitly convertible to `pointer`.
     * @param ptr a pointer.
     */
    template <typename U,
              typename std::enable_if<std::is_constructible<pointer, U>::value
                                  &&! std::is_convertible<U, pointer>::value>::type * = nullptr>
    constexpr explicit
    optional_ref (U&& ptr) noexcept
      : m_ptr (static_cast<pointer> (ptr))
    { }

    /**
     * Constructor
     *
     * A copy constructor from another optional_ref for the case
     * where `U*` is implicitly convertible to type `pointer`.
     *
     * @tparam U a referenced value type.
     * @param other an optional_ref whose pointer is implicitly
     *              convertible to type `pointer`.
     */
    template <typename U,
              typename std::enable_if<std::is_constructible<pointer, U *>::value &&
                                      std::is_convertible<U *, pointer>::value>::type * = nullptr>
    constexpr /* implicit */
    optional_ref (const optional_ref<U>& other) noexcept
      : m_ptr (other.get_pointer ())
    { }

    /**
     * Constructor
     *
     * A copy constructor from another optional_ref for the case
     * where `pointer` is explicitly constructable from `U*`.
     *
     * @tparam U a referenced value type.
     * @param ref an optional_ref which contains a pointer from
     *            which `pointer` may be explicitly constructed.
     */
    template <typename U,
              typename std::enable_if<std::is_constructible<pointer, U*>::value
                                  &&! std::is_convertible<U*, pointer>::value>::type * = nullptr>
    constexpr explicit
    optional_ref (const optional_ref<U>& other) noexcept
      : m_ptr (static_cast<pointer> (other.get_pointer ()))
    { }

    /**
     * Returns a pointer representation of the reference.
     *
     * The pointer is the internal representation of the reference.
     * This function never fails.
     *
     *
     * @return a pointer representation of the reference.
     */
    GCH_NODISCARD constexpr
    pointer
    get_pointer (void) const noexcept
    {
      return m_ptr;
    }

    /**
     * Returns the reference.
     *
     * Dereferences `nullptr` if `*this` has no value.
     *
     * @return the stored reference.
     */
    GCH_NODISCARD constexpr
    reference
    operator* (void) const noexcept
    {
      return *get_pointer ();
    }

    /**
     * Returns a pointer to the value.
     *
     * Never fails. The return is forwarded from `get_pointer`.
     *
     * @return a pointer to the value.
     */
    GCH_NODISCARD constexpr
    pointer
    operator-> (void) const noexcept
    {
      return get_pointer ();
    }

    /**
     * Checks if the `*this` contains a value.
     *
     * Internally, this is just a check against `nullptr`.
     *
     * @return whether this `*this` contains a value.
     */
    GCH_NODISCARD constexpr
    bool
    has_value (void) const noexcept
    {
      return m_ptr != nullptr;
    }

    /**
     * Checks if the `*this` contains a value.
     *
     * The return is forwarded from `has_value ()`.
     *
     * @return whether this `*this` contains a value.
     */
    GCH_NODISCARD constexpr explicit
    operator bool (void) const noexcept
    {
      return has_value ();
    }

    /**
     * Returns the contained reference, while checking whether it exists.
     *
     * @throws bad_optional_access when `*this` does not contain a value.
     *
     * @return the contained reference.
     */
    GCH_NODISCARD GCH_CPP14_CONSTEXPR
    reference
    value (void) const
    {
      if (! has_value ())
        throw bad_optional_access();
      return *m_ptr;
    }

    /**
     * Returns the value, or a default.
     *
     * Returns the value, or a specified default if `*this` does
     * not contain a value.
     *
     * @tparam U a reference type convertible to `reference`.
     * @param default_value the value returned if `*this`
     *                      does not contain a value.
     * @return the stored reference, or `default_value`
     *         if `*this` does not contain a value.
     */
    template <typename U>
    GCH_NODISCARD constexpr
    reference
    value_or (U& default_value) const noexcept
    {
      return has_value () ? *m_ptr : static_cast<reference> (default_value);
    }

    /**
     * Returns the value, or a default.
     *
     * Returns the value, or a specified default if `*this`
     * does not contain a value. In this case, the default
     * is an rvalue reference of type `Value`. Note that
     * we're using temporary lifetime extension here, so
     * don't use the return beyond the scope of the enclosing
     * expression to this function call.
     *
     * @param default_value the value returned if `*this`
     *                      does not contain a value.
     * @return the stored reference, or `default_value`
     *         if `*this` does not contain a value.
     */
    GCH_NODISCARD constexpr
    const_reference
    value_or (const Value&& default_value) const noexcept
    {
      return has_value () ? *m_ptr : default_value;
    }

    /**
     * A deleted version for convertible rvalue references.
     *
     * We can't return the converted temporary because
     * its lifetime isn't extended.
     */
    template <typename U,
              typename = typename std::enable_if<std::is_constructible<pointer, U*>::value>::type>
    GCH_NODISCARD constexpr
    const_reference
    value_or (const U&& default_value) const noexcept = delete;

    /**
     * Swap the contained reference with that of `other`.
     *
     * @param other a reference to another `optional_ref`.
     */
    GCH_SWAP_CONSTEXPR
    void
    swap (optional_ref& other) noexcept
    {
      using std::swap;
      swap (this->m_ptr, other.m_ptr);
    }

    /**
     * Reset the contained reference.
     *
     * Internally, sets the pointer to `nullptr`.
     */
    GCH_CPP14_CONSTEXPR
    void
    reset (void) noexcept
    {
      m_ptr = nullptr;
    }

    /**
     * Sets the contained reference.
     *
     * Internally, sets the a pointer to the address of the referenced value.
     *
     * @tparam U a reference type convertible to `reference`.
     * @param ref an lvalue reference.
     * @return the argument `ref`.
     */
    template <typename U,
              typename = typename std::enable_if<constructible_from_pointer_to<U>::value>::type>
    GCH_CPP14_CONSTEXPR
    reference
    emplace (U& ref) noexcept
    {
      return *(m_ptr = static_cast<pointer> (&ref));
    }

    /**
     * A deleted version for convertible rvalue references.
     *
     * We don't want to allow rvalue references because the internal pointer
     * does not sustain the object lifetime.
     */
    template <typename U,
              typename = typename std::enable_if<constructible_from_pointer_to<U>::value>::type>
    reference
    emplace (const U&&) = delete;

    /**
     * Sets the contained pointer.
     *
     * Sets the pointer to the pointer contained by another optional_ref,
     * where `pointer` is constructible from `U *`.
     *
     * @tparam U a referenced value type.
     * @param other a optional_ref which contains a pointer from
     *              which `pointer` may be constructed.
     * @return a reference to the contained pointer.
     */
    template <typename U,
              typename = typename std::enable_if<std::is_constructible<pointer, U *>::value>>
    GCH_CPP14_CONSTEXPR
    reference
    emplace (const optional_ref<U>& other) noexcept
    {
      return *(m_ptr = static_cast<pointer> (other.get ()));
    }

    /**
     * Compares reference addresses.
     *
     * Does a pointer comparison between the argument and
     * the stored reference.
     *
     * @tparam U a reference type convertible to `reference`.
     * @param ref an lvalue reference.
     * @return whether `*this` contains `ref`
     */
    template <typename U,
              typename = typename std::enable_if<constructible_from_pointer_to<U>::value>::type>
    GCH_NODISCARD constexpr
    bool
    contains (U& ref) const noexcept
    {
      return static_cast<pointer> (&ref) == m_ptr;
    }

    /**
     * A deleted version for rvalue references.
     *
     * Using `contains` with an rvalue reference is obviously
     * incorrectly written, so we might as well delete it.
     */
    template <typename U,
              typename = typename std::enable_if<constructible_from_pointer_to<U>::value>::type>
    bool
    contains (const U&&) const noexcept = delete;

  private:
    /**
     * A pointer to the value.
     *
     * We use a pointer representation instead of an actual
     * reference so we can have rebind semantics.
     */
    pointer m_ptr { nullptr };
  };

  /**
   * An equality comparison function.
   *
   * We compare by address, not by value.
   *
   * @tparam T the value type of `l`.
   * @tparam U the value type of `r`.
   * @param l an `optional_ref`.
   * @param r an `optional_ref`.
   * @return the result of the equality comparison.
   *
   * @see std::optional::operator==
   */
  template <typename T, typename U>
  GCH_NODISCARD constexpr
  auto
  operator== (const optional_ref<T>& l, const optional_ref<U>& r)
    noexcept (noexcept (l.get_pointer () == r.get_pointer ()))
    -> decltype (l.get_pointer () == r.get_pointer ())
  {
    return (l.has_value () == r.has_value ()) && (! l.has_value ()
                                                  || (l.get_pointer () == r.get_pointer ()));
  }

  /**
   * An inequality comparison function.
   *
   * We compare by address, not by value.
   *
   * @tparam T the value type of `l`.
   * @tparam U the value type of `r`.
   * @param l an `optional_ref`.
   * @param r an `optional_ref`.
   * @return the result of the inequality comparison.
   *
   * @see std::optional::operator!=
   */
  template <typename T, typename U>
  GCH_NODISCARD constexpr
  auto
  operator!= (const optional_ref<T>& l, const optional_ref<U>& r)
    noexcept (noexcept (l.get_pointer () != r.get_pointer ()))
    -> decltype (l.get_pointer () != r.get_pointer ())
  {
    return (l.has_value () != r.has_value ()) || (l.has_value ()
                                                  && (l.get_pointer () != r.get_pointer ()));
  }

  /**
   * An less-than comparison function.
   *
   * We compare by address, not by value.
   *
   * @tparam T the value type of `l`.
   * @tparam U the value type of `r`.
   * @param l an `optional_ref`.
   * @param r an `optional_ref`.
   * @return the result of the less-than comparison.
   *
   * @see std::optional::operator<
   */
  template <typename T, typename U>
  GCH_NODISCARD constexpr
  auto
  operator< (const optional_ref<T>& l, const optional_ref<U>& r)
    noexcept (noexcept (l.get_pointer () < r.get_pointer ()))
    -> decltype (l.get_pointer () < r.get_pointer ())
  {
    return r.has_value () && (! l.has_value () || (l.get_pointer () < r.get_pointer ()));
  }

  /**
   * A greater-than comparison function.
   *
   * We compare by address, not by value.
   *
   * @tparam T the value type of `l`.
   * @tparam U the value type of `r`.
   * @param l an `optional_ref`.
   * @param r an `optional_ref`.
   * @return the result of the greater-than comparison.
   *
   * @see std::optional::operator>
   */
  template <typename T, typename U>
  GCH_NODISCARD constexpr
  auto
  operator> (const optional_ref<T>& l, const optional_ref<U>& r)
    noexcept (noexcept (l.get_pointer () > r.get_pointer ()))
    -> decltype (l.get_pointer () > r.get_pointer ())
  {
    return l.has_value () && (! r.has_value () || (l.get_pointer () > r.get_pointer ()));
  }

  /**
   * A less-than-equal comparison function.
   *
   * We compare by address, not by value.
   *
   * @tparam T the value type of `l`.
   * @tparam U the value type of `r`.
   * @param l an `optional_ref`.
   * @param r an `optional_ref`.
   * @return the result of the less-than-equal comparison.
   *
   * @see std::optional::operator<=
   */
  template <typename T, typename U>
  GCH_NODISCARD constexpr
  auto
  operator<= (const optional_ref<T>& l, const optional_ref<U>& r)
    noexcept (noexcept (l.get_pointer () <= r.get_pointer ()))
    -> decltype (l.get_pointer () <= r.get_pointer ())
  {
    return ! l.has_value () || (r.has_value () && (l.get_pointer () <= r.get_pointer ()));
  }

  /**
   * A greater-than-equal comparison function.
   *
   * We compare by address, not by value.
   *
   * @tparam T the value type of `l`.
   * @tparam U the value type of `r`.
   * @param l an `optional_ref`.
   * @param r an `optional_ref`.
   * @return the result of the greater-than-equal comparison.
   *
   * @see std::optional::operator>=
   */
  template <typename T, typename U>
  GCH_NODISCARD constexpr
  auto
  operator>= (const optional_ref<T>& l, const optional_ref<U>& r)
    noexcept (noexcept (l.get_pointer () >= r.get_pointer ()))
    -> decltype (l.get_pointer () >= r.get_pointer ())
  {
    return ! r.has_value () || (l.has_value () && (l.get_pointer () >= r.get_pointer ()));
  }

#ifdef GCH_LIB_THREE_WAY_COMPARISON

  /**
   * A three-way comparison function.
   *
   * We compare by address, not by value.
   *
   * @tparam T the value type of `l`.
   * @tparam U the value type of `r`.
   * @param l an `optional_ref`.
   * @param r an `optional_ref`.
   * @return the result of the three-way comparison.
   *
   * @see std::optional::operator<=>
   */
  template <typename T, typename U>
  GCH_NODISCARD constexpr
  auto
  operator<=> (const optional_ref<T>& l, const optional_ref<U>& r)
    noexcept (noexcept (l.get_pointer () <=> r.get_pointer ()))
    -> std::compare_three_way_result_t<decltype (l.get_pointer ()), decltype (r.get_pointer ())>
  {
    return (l.has_value () && r.has_value ()) ? (l.get_pointer () <=> r.get_pointer ())
                                              : (bool (l) <=> bool (r));
  }

#endif

  /**
   * An equality comparison function.
   *
   * We compare by address, not by value.
   *
   * @tparam T the value type of `l`.
   * @param l an `optional_ref`.
   * @param r a `nullopt_t`.
   * @return the result of the equality comparison.
   *
   * @see std::optional::operator==
   */
  template <typename T>
  GCH_NODISCARD constexpr
  bool
  operator== (const optional_ref<T>& l, nullopt_t) noexcept
  {
    return ! l.has_value ();
  }

#ifdef GCH_LIB_THREE_WAY_COMPARISON

  /**
   * A three-way comparison function.
   *
   * We compare by address, not by value.
   *
   * @tparam T the value type of `r`.
   * @param l an `optional_ref`.
   * @param r a `nullopt_t`.
   * @return the result of the three-way comparison.
   *
   * @see std::optional::operator<=>
   */
  template <typename T> GCH_NODISCARD
  constexpr std::strong_ordering operator<=> (const optional_ref<T>& l, nullopt_t) noexcept
  {
    return l.has_value () <=> false;
  }

#else

  /**
   * An equality comparison function.
   *
   * We compare by address, not by value.
   *
   * @tparam T the value type of `r`.
   * @param l a `nullopt_t`.
   * @param r an `optional_ref`.
   * @return the result of the equality comparison.
   *
   * @see std::optional::operator==
   */
  template <typename T>
  GCH_NODISCARD constexpr
  bool
  operator== (nullopt_t, const optional_ref<T>& r) noexcept
  {
    return ! r.has_value ();
  }

  /**
   * An inequality comparison function.
   *
   * We compare by address, not by value.
   *
   * @tparam T the value type of `l`.
   * @param l an `optional_ref`.
   * @param r a `nullopt_t`.
   * @return the result of the inequality comparison.
   *
   * @see std::optional::operator!=
   */
  template <typename T>
  GCH_NODISCARD constexpr
  bool
  operator!= (const optional_ref<T>& l, nullopt_t) noexcept
  {
    return l.has_value ();
  }

  /**
   * An inequality comparison function.
   *
   * We compare by address, not by value.
   *
   * @tparam T the value type of `r`.
   * @param l a `nullopt_t`.
   * @param r an `optional_ref`.
   * @return the result of the inequality comparison.
   *
   * @see std::optional::operator!=
   */
  template <typename T>
  GCH_NODISCARD constexpr
  bool
  operator!= (nullopt_t, const optional_ref<T>& r) noexcept
  {
    return r.has_value ();
  }

  /**
   * A less-than comparison function.
   *
   * We compare by address, not by value.
   *
   * @tparam T the value type of `l`.
   * @param l an `optional_ref`.
   * @param r a `nullopt_t`.
   * @return the result of the less-than comparison.
   *
   * @see std::optional::operator<
   */
  template <typename T>
  GCH_NODISCARD constexpr
  bool
  operator< (const optional_ref<T>&, nullopt_t) noexcept
  {
    return false;
  }

  /**
   * A less-than comparison function.
   *
   * We compare by address, not by value.
   *
   * @tparam T the value type of `r`.
   * @param l a `nullopt_t`.
   * @param r an `optional_ref`.
   * @return the result of the less-than comparison.
   *
   * @see std::optional::operator<
   */
  template <typename T>
  GCH_NODISCARD constexpr
  bool
  operator< (nullopt_t, const optional_ref<T>& r) noexcept
  {
    return r.has_value ();
  }

  /**
   * A greater-than comparison function.
   *
   * We compare by address, not by value.
   *
   * @tparam T the value type of `l`.
   * @param l an `optional_ref`.
   * @param r a `nullopt_t`.
   * @return the result of the greater-than comparison.
   *
   * @see std::optional::operator>
   */
  template <typename T>
  GCH_NODISCARD constexpr
  bool
  operator> (const optional_ref<T>& l, nullopt_t) noexcept
  {
    return l.has_value ();
  }

  /**
   * A greater-than comparison function.
   *
   * We compare by address, not by value.
   *
   * @tparam T the value type of `r`.
   * @param l a `nullopt_t`.
   * @param r an `optional_ref`.
   * @return the result of the greater-than comparison.
   *
   * @see std::optional::operator>
   */
  template <typename T>
  GCH_NODISCARD constexpr
  bool
  operator> (nullopt_t, const optional_ref<T>&) noexcept
  {
    return false;
  }

  /**
   * A less-than-equal comparison function.
   *
   * We compare by address, not by value.
   *
   * @tparam T the value type of `l`.
   * @param l an `optional_ref`.
   * @param r a `nullopt_t`.
   * @return the result of the less-than-equal comparison.
   *
   * @see std::optional::operator<=
   */
  template <typename T>
  GCH_NODISCARD constexpr
  bool
  operator<= (const optional_ref<T>& l, nullopt_t) noexcept
  {
    return ! l.has_value ();
  }

  /**
   * A less-than-equal comparison function.
   *
   * We compare by address, not by value.
   *
   * @tparam T the value type of `r`.
   * @param l a `nullopt_t`.
   * @param r an `optional_ref`.
   * @return the result of the less-than-equal comparison.
   *
   * @see std::optional::operator<=
   */
  template <typename T>
  GCH_NODISCARD constexpr
  bool
  operator<= (nullopt_t, const optional_ref<T>&) noexcept
  {
    return true;
  }

  /**
   * A greater-than-equal comparison function.
   *
   * We compare by address, not by value.
   *
   * @tparam T the value type of `l`.
   * @param l an `optional_ref`.
   * @param r a `nullopt_t`.
   * @return the result of the greater-than-equal comparison.
   *
   * @see std::optional::operator>=
   */
  template <typename T>
  GCH_NODISCARD constexpr
  bool
  operator>= (const optional_ref<T>&, nullopt_t) noexcept
  {
    return true;
  }

  /**
   * A greater-than-equal comparison function.
   *
   * We compare by address, not by value.
   *
   * @tparam T the value type of `r`.
   * @param l a `nullopt_t`.
   * @param r an `optional_ref`.
   * @return the result of the greater-than-equal comparison.
   *
   * @see std::optional::operator>=
   */
  template <typename T>
  GCH_NODISCARD constexpr
  bool
  operator>= (nullopt_t, const optional_ref<T>& r) noexcept
  {
    return ! r.has_value ();
  }

#endif

  /**
 * An equality comparison function.
 *
 * We compare by address, not by value.
 *
 * @tparam T the value type of `l`.
 * @param l an `optional_ref`.
 * @param r a `std::nullptr_t`.
 * @return the result of the equality comparison.
 *
 * @see std::optional::operator==
 */
  template <typename T>
  GCH_NODISCARD constexpr
  bool
  operator== (const optional_ref<T>& l, std::nullptr_t) noexcept
  {
    return ! l.has_value ();
  }

#ifdef GCH_LIB_THREE_WAY_COMPARISON

  /**
   * A three-way comparison function.
   *
   * We compare by address, not by value.
   *
   * @tparam T the value type of `r`.
   * @param l an `optional_ref`.
   * @param r a `std::nullptr_t`.
   * @return the result of the three-way comparison.
   *
   * @see std::optional::operator<=>
   */
  template <typename T> GCH_NODISCARD
  constexpr std::strong_ordering operator<=> (const optional_ref<T>& l, std::nullptr_t) noexcept
  {
    return l.has_value () <=> false;
  }

#else

  /**
   * An equality comparison function.
   *
   * We compare by address, not by value.
   *
   * @tparam T the value type of `r`.
   * @param l a `std::nullptr_t`.
   * @param r an `optional_ref`.
   * @return the result of the equality comparison.
   *
   * @see std::optional::operator==
   */
  template <typename T>
  GCH_NODISCARD constexpr
  bool
  operator== (std::nullptr_t, const optional_ref<T>& r) noexcept
  {
    return ! r.has_value ();
  }

  /**
   * An inequality comparison function.
   *
   * We compare by address, not by value.
   *
   * @tparam T the value type of `l`.
   * @param l an `optional_ref`.
   * @param r a `std::nullptr_t`.
   * @return the result of the inequality comparison.
   *
   * @see std::optional::operator!=
   */
  template <typename T>
  GCH_NODISCARD constexpr
  bool
  operator!= (const optional_ref<T>& l, std::nullptr_t) noexcept
  {
    return l.has_value ();
  }

  /**
   * An inequality comparison function.
   *
   * We compare by address, not by value.
   *
   * @tparam T the value type of `r`.
   * @param l a `std::nullptr_t`.
   * @param r an `optional_ref`.
   * @return the result of the inequality comparison.
   *
   * @see std::optional::operator!=
   */
  template <typename T>
  GCH_NODISCARD constexpr
  bool
  operator!= (std::nullptr_t, const optional_ref<T>& r) noexcept
  {
    return r.has_value ();
  }

  /**
   * A less-than comparison function.
   *
   * We compare by address, not by value.
   *
   * @tparam T the value type of `l`.
   * @param l an `optional_ref`.
   * @param r a `std::nullptr_t`.
   * @return the result of the less-than comparison.
   *
   * @see std::optional::operator<
   */
  template <typename T>
  GCH_NODISCARD constexpr
  bool
  operator< (const optional_ref<T>&, std::nullptr_t) noexcept
  {
    return false;
  }

  /**
   * A less-than comparison function.
   *
   * We compare by address, not by value.
   *
   * @tparam T the value type of `r`.
   * @param l a `std::nullptr_t`.
   * @param r an `optional_ref`.
   * @return the result of the less-than comparison.
   *
   * @see std::optional::operator<
   */
  template <typename T>
  GCH_NODISCARD constexpr
  bool
  operator< (std::nullptr_t, const optional_ref<T>& r) noexcept
  {
    return r.has_value ();
  }

  /**
   * A greater-than comparison function.
   *
   * We compare by address, not by value.
   *
   * @tparam T the value type of `l`.
   * @param l an `optional_ref`.
   * @param r a `std::nullptr_t`.
   * @return the result of the greater-than comparison.
   *
   * @see std::optional::operator>
   */
  template <typename T>
  GCH_NODISCARD constexpr
  bool
  operator> (const optional_ref<T>& l, std::nullptr_t) noexcept
  {
    return l.has_value ();
  }

  /**
   * A greater-than comparison function.
   *
   * We compare by address, not by value.
   *
   * @tparam T the value type of `r`.
   * @param l a `std::nullptr_t`.
   * @param r an `optional_ref`.
   * @return the result of the greater-than comparison.
   *
   * @see std::optional::operator>
   */
  template <typename T>
  GCH_NODISCARD constexpr
  bool
  operator> (std::nullptr_t, const optional_ref<T>&) noexcept
  {
    return false;
  }

  /**
   * A less-than-equal comparison function.
   *
   * We compare by address, not by value.
   *
   * @tparam T the value type of `l`.
   * @param l an `optional_ref`.
   * @param r a `std::nullptr_t`.
   * @return the result of the less-than-equal comparison.
   *
   * @see std::optional::operator<=
   */
  template <typename T>
  GCH_NODISCARD constexpr
  bool
  operator<= (const optional_ref<T>& l, std::nullptr_t) noexcept
  {
    return ! l.has_value ();
  }

  /**
   * A less-than-equal comparison function.
   *
   * We compare by address, not by value.
   *
   * @tparam T the value type of `r`.
   * @param l a `std::nullptr_t`.
   * @param r an `optional_ref`.
   * @return the result of the less-than-equal comparison.
   *
   * @see std::optional::operator<=
   */
  template <typename T>
  GCH_NODISCARD constexpr
  bool
  operator<= (std::nullptr_t, const optional_ref<T>&) noexcept
  {
    return true;
  }

  /**
   * A greater-than-equal comparison function.
   *
   * We compare by address, not by value.
   *
   * @tparam T the value type of `l`.
   * @param l an `optional_ref`.
   * @param r a `std::nullptr_t`.
   * @return the result of the greater-than-equal comparison.
   *
   * @see std::optional::operator>=
   */
  template <typename T>
  GCH_NODISCARD constexpr
  bool
  operator>= (const optional_ref<T>&, std::nullptr_t) noexcept
  {
    return true;
  }

  /**
   * A greater-than-equal comparison function.
   *
   * We compare by address, not by value.
   *
   * @tparam T the value type of `r`.
   * @param l a `std::nullptr_t`.
   * @param r an `optional_ref`.
   * @return the result of the greater-than-equal comparison.
   *
   * @see std::optional::operator>=
   */
  template <typename T>
  GCH_NODISCARD constexpr
  bool
  operator>= (std::nullptr_t, const optional_ref<T>& r) noexcept
  {
    return ! r.has_value ();
  }

#endif

  /**
   * An equality comparison function.
   *
   * We compare by address, not by value.
   *
   * @tparam T the value type of `l`.
   * @tparam U a value type which is comparable to `T *`.
   * @param l an `optional_ref`.
   * @param r a comparable pointer.
   * @return the result of the equality comparison.
   *
   * @see std::optional::operator==
   */
  template <typename T, typename U>
  GCH_NODISCARD constexpr
  auto
  operator== (const optional_ref<T>& l, const U *r)
    noexcept (noexcept (l.get_pointer () == r)) -> decltype (l.get_pointer () == r)
  {
    return (l.has_value () && (l.get_pointer () == r));
  }

  /**
   * An equality comparison function.
   *
   * We compare by address, not by value.
   *
   * @tparam T the value type of `r`.
   * @tparam U a value type which is comparable to `T *`.
   * @param l a comparable pointer.
   * @param r an `optional_ref`.
   * @return the result of the equality comparison.
   *
   * @see std::optional::operator==
   */
  template <typename T, typename U>
  GCH_NODISCARD constexpr
  auto
  operator== (const U *l, const optional_ref<T>& r)
    noexcept (noexcept (l == r.get_pointer ())) -> decltype (l == r.get_pointer ())
  {
    return (r.has_value () && (l == r.get_pointer ()));
  }

  /**
   * An inequality comparison function.
   *
   * We compare by address, not by value.
   *
   * @tparam T the value type of `l`.
   * @tparam U a value type which is comparable to `T *`.
   * @param l an `optional_ref`.
   * @param r a comparable pointer.
   * @return the result of the inequality comparison.
   *
   * @see std::optional::operator!=
   */
  template <typename T, typename U>
  GCH_NODISCARD constexpr
  auto
  operator!= (const optional_ref<T>& l, const U *r)
    noexcept (noexcept (l.get_pointer () != r)) -> decltype (l.get_pointer () != r)
  {
    return (! l.has_value () || (l.get_pointer () != r));
  }

  /**
   * An inequality comparison function.
   *
   * We compare by address, not by value.
   *
   * @tparam T the value type of `r`.
   * @tparam U a value type which is comparable to `T *`.
   * @param l a comparable pointer.
   * @param r an `optional_ref`.
   * @return the result of the inequality comparison.
   *
   * @see std::optional::operator!=
   */
  template <typename T, typename U>
  GCH_NODISCARD constexpr
  auto
  operator!= (const U *l, const optional_ref<T>& r)
    noexcept (noexcept (l != r.get_pointer ())) -> decltype (l != r.get_pointer ())
  {
    return (! r.has_value () || (l != r.get_pointer ()));
  }

  /**
   * A less-than comparison function.
   *
   * We compare by address, not by value.
   *
   * @tparam T the value type of `l`.
   * @tparam U a value type which is comparable to `T *`.
   * @param l an `optional_ref`.
   * @param r a comparable pointer.
   * @return the result of the less-than comparison.
   *
   * @see std::optional::operator<
   */
  template <typename T, typename U>
  GCH_NODISCARD constexpr
  auto
  operator< (const optional_ref<T>& l, const U *r)
    noexcept (noexcept (l.get_pointer () < r)) -> decltype (l.get_pointer () < r)
  {
    return (! l.has_value () || (l.get_pointer () < r));
  }

  /**
   * A less-than comparison function.
   *
   * We compare by address, not by value.
   *
   * @tparam T the value type of `r`.
   * @tparam U a value type which is comparable to `T *`.
   * @param l a comparable pointer.
   * @param r an `optional_ref`.
   * @return the result of the less-than comparison.
   *
   * @see std::optional::operator<
   */
  template <typename T, typename U>
  GCH_NODISCARD constexpr
  auto
  operator< (const U *l, const optional_ref<T>& r)
    noexcept (noexcept (l < r.get_pointer ())) -> decltype (l < r.get_pointer ())
  {
    return (r.has_value () && (l < r.get_pointer ()));
  }

  /**
   * A greater-than comparison function.
   *
   * We compare by address, not by value.
   *
   * @tparam T the value type of `l`.
   * @tparam U a value type which is comparable to `T *`.
   * @param l an `optional_ref`.
   * @param r a comparable pointer.
   * @return the result of the greater-than comparison.
   *
   * @see std::optional::operator>
   */
  template <typename T, typename U>
  GCH_NODISCARD constexpr
  auto
  operator> (const optional_ref<T>& l, const U *r)
    noexcept (noexcept (l.get_pointer () > r)) -> decltype (l.get_pointer () > r)
  {
    return (l.has_value () && (l.get_pointer () > r));
  }

  /**
   * A greater-than comparison function.
   *
   * We compare by address, not by value.
   *
   * @tparam T the value type of `r`.
   * @tparam U a value type which is comparable to `T *`.
   * @param l a comparable pointer.
   * @param r an `optional_ref`.
   * @return the result of the greater-than comparison.
   *
   * @see std::optional::operator>
   */
  template <typename T, typename U>
  GCH_NODISCARD constexpr
  auto
  operator> (const U *l, const optional_ref<T>& r)
    noexcept (noexcept (l > r.get_pointer ())) -> decltype (l > r.get_pointer ())
  {
    return (! r.has_value () || (l > r.get_pointer ()));
  }

  /**
   * A less-than-equal comparison function.
   *
   * We compare by address, not by value.
   *
   * @tparam T the value type of `l`.
   * @tparam U a value type which is comparable to `T *`.
   * @param l an `optional_ref`.
   * @param r a comparable pointer.
   * @return the result of the less-than-equal comparison.
   *
   * @see std::optional::operator<=
   */
  template <typename T, typename U>
  GCH_NODISCARD constexpr
  auto
  operator<= (const optional_ref<T>& l, const U *r)
    noexcept (noexcept (l.get_pointer () <= r)) -> decltype (l.get_pointer () <= r)
  {
    return (! l.has_value () || (l.get_pointer () <= r));
  }

  /**
   * A less-than-equal comparison function.
   *
   * We compare by address, not by value.
   *
   * @tparam T the value type of `r`.
   * @tparam U a value type which is comparable to `T *`.
   * @param l a comparable pointer.
   * @param r an `optional_ref`.
   * @return the result of the less-than-equal comparison.
   *
   * @see std::optional::operator<=
   */
  template <typename T, typename U>
  GCH_NODISCARD constexpr
  auto
  operator<= (const U *l, const optional_ref<T>& r)
    noexcept (noexcept (l <= r.get_pointer ())) -> decltype (l <= r.get_pointer ())
  {
    return (r.has_value () && (l <= r.get_pointer ()));
  }

  /**
   * A greater-than-equal comparison function.
   *
   * We compare by address, not by value.
   *
   * @tparam T the value type of `l`.
   * @tparam U a value type which is comparable to `T *`.
   * @param l an `optional_ref`.
   * @param r a comparable pointer.
   * @return the result of the greater-than-equal comparison.
   *
   * @see std::optional::operator>=
   */
  template <typename T, typename U>
  GCH_NODISCARD constexpr
  auto
  operator>= (const optional_ref<T>& l, const U *r)
    noexcept (noexcept (l.get_pointer () >= r)) -> decltype (l.get_pointer () >= r)
  {
    return (l.has_value () && (l.get_pointer () >= r));
  }

  /**
   * A greater-than-equal comparison function.
   *
   * We compare by address, not by value.
   *
   * @tparam T the value type of `r`.
   * @tparam U a value type which is comparable to `T *`.
   * @param l a comparable pointer.
   * @param r an `optional_ref`.
   * @return the result of the greater-than-equal comparison.
   *
   * @see std::optional::operator>=
   */
  template <typename T, typename U>
  GCH_NODISCARD constexpr
  auto
  operator>= (const U *l, const optional_ref<T>& r)
    noexcept (noexcept (l >= r.get_pointer ())) -> decltype (l >= r.get_pointer ())
  {
    return (! r.has_value () || (l >= r.get_pointer ()));
  }

#ifdef GCH_LIB_THREE_WAY_COMPARISON

  /**
   * A three-way comparison function.
   *
   * We compare by address, not by value.
   *
   * @tparam T the value type of `l`.
   * @tparam U a value type which is comparable to `T *`.
   * @param l an `optional_ref`.
   * @param r a comparable pointer.
   * @return the result of the three-way comparison.
   *
   * @see std::optional::operator<=>
   */
  template <typename T, typename U>
  GCH_NODISCARD constexpr
  auto
  operator<=> (const optional_ref<T>& l, const U *r)
    noexcept (noexcept (l.get_pointer () <=> r))
    -> std::compare_three_way_result_t<decltype (l.get_pointer ()), decltype (r)>
  {
    return l.has_value () ? (l.get_pointer () <=> r) : std::strong_ordering::less;
  }

#endif

  /**
   * A swap function.
   *
   * Swaps the two `optional_ref`s of the same type.
   *
   * @tparam T the value type pointed to by the `optional_ref`s
   * @param l an `optional_ref`.
   * @param r an `optional_ref`.
   */
  template <typename T>
  inline GCH_SWAP_CONSTEXPR
  void
  swap (optional_ref<T>& l, optional_ref<T>& r) noexcept
  {
    l.swap (r);
  }

  /**
   * An optional_ref creation function.
   *
   * Creates an `optional_ref` with the specified argument.
   *
   * @tparam U a forwarded type.
   * @param ref a forwarded value.
   * @return an `optional_ref` created from the argument.
   *
   * @see std::make_optional
   */
  template <typename U>
  GCH_NODISCARD constexpr
  optional_ref<typename std::remove_reference<U>::type>
  make_optional_ref (U&& ref) noexcept
  {
    return optional_ref<typename std::remove_reference<U>::type> { std::forward<U> (ref) };
  }

#ifdef GCH_CTAD_SUPPORT
  template <typename U> optional_ref (U&&) -> optional_ref<std::remove_reference_t<U>>;
#endif

}

namespace std
{

  /**
   * A specialization of `std::hash` for `gch::optional_ref`.
   *
   * @tparam T the value type of `gch::optional_ref`.
   */
  template <typename T>
  struct hash<gch::optional_ref<T>>
  {
    /**
     * An invokable operator.
     *
     * We just do a noop pointer hash (which is unique).
     *
     * @param opt_ref a reference to a value of type `gch::optional_ref`.
     * @return a hash of the argument.
     */
    std::size_t
    operator() (const gch::optional_ref<T>& opt_ref) const noexcept
    {
      return reinterpret_cast<std::size_t> (opt_ref.get_pointer ());
    }
  };

}



#endif
