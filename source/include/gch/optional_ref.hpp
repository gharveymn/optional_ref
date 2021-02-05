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

#ifdef __clang__
#  ifndef GCH_CLANG
#    define GCH_CLANG
#  endif
#  if defined (__cplusplus) && __cplusplus >= 201703L
#    ifndef GCH_CLANG_17
#      define GCH_CLANG_17
#    endif
#  endif
#endif

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

#ifndef GCH_CPP20_CONSTEVAL
#  if defined (__cpp_consteval) && __cpp_consteval >= 201811L
#    define GCH_CPP20_CONSTEVAL consteval
#  else
#    define GCH_CPP20_CONSTEVAL constexpr
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

#if defined (__cpp_variable_templates) && __cpp_variable_templates >= 201304
#  ifndef GCH_VARIABLE_TEMPLATES
#    define GCH_VARIABLE_TEMPLATES
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
    struct create_tag { };
    static constexpr create_tag create { };

    constexpr explicit
    nullopt_t (create_tag) noexcept
    { }

    constexpr /* implicit */
    nullopt_t (std::nullptr_t) noexcept
    { }
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
    const char *
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

    using value_type      = Value;         /*!< The value type of the stored reference */
    using reference       = Value&;        /*!< The reference type to be wrapped       */
    using pointer         = Value *;       /*!< The pointer type to the value type     */
    using const_reference = const Value&;  /*!< A constant reference to `Value`        */
    using const_pointer   = const Value *; /*!< A constant pointer to `Value`          */

  private:
    template <typename U>
    using constructible_from_pointer_to =
      std::is_constructible<pointer, decltype (&std::declval<U&> ())>;

    template <typename U>
    using pointer_to_is_convertible =
      std::is_convertible<decltype (&std::declval<U&> ()), pointer>;

  public:
    // For the record, I don't think the explicit conversions are even possible
    // since we are only using raw pointers, but I can't find a definitive
    // reference to that fact, so they stay.

    /**
     * Constructor
     *
     * A default constructor.
     *
     * The `optional_ref` has no value after default
     * construction.
     */
    optional_ref (void) noexcept = default;

    /**
     * Constructor
     *
     * A copy constructor.
     *
     * If `other` contains a value, then that value
     * is copied into `*this`.
     *
     * Note: TriviallyCopyable.
     */
    optional_ref (const optional_ref& other) noexcept = default;

    /**
     * Constructor
     *
     * A move constructor.
     *
     * If `other` contains a value, then that value
     * is copied into `*this`. It is implementation
     * defined if `other` contains a value after this.
     *
     * Note: TriviallyCopyable.
     */
    optional_ref (optional_ref&& other) noexcept = default;

    /**
     * Assignment operator
     *
     * A copy-assignment operator.
     *
     * The resultant state of `*this` is equivalent
     * to that of `other`.
     *
     * Note: TriviallyCopyable.
     *
     * @return `*this`
     */
    optional_ref&
    operator= (const optional_ref& other) noexcept = default;

    /**
     * Assignment operator
     *
     * A move-assignment operator.
     *
     * Sets the state of `*this` to that of `other`.
     * It is implementation defined if `other` contains
     * a value after this.
     *
     * Note: TriviallyCopyable.
     *
     * @return `*this`
     */
    optional_ref&
    operator= (optional_ref&& other) noexcept = default;

    /**
     * Destructor
     *
     * A trivial destructor.
     *
     * Note: TriviallyCopyable.
     *
     */
    ~optional_ref (void) noexcept = default;

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
     * A converting constructor for types implicitly
     * convertible to `pointer`. This is so we don't
     * need to dereference or inspect pointers before
     * creation; we can just use the pointer directly.
     *
     * @tparam U a type implicitly convertible to `pointer`.
     * @param ptr a pointer.
     */
    template <typename Ptr,
              typename std::enable_if<std::is_constructible<pointer, Ptr>::value
                                  &&  std::is_convertible<Ptr, pointer>::value>::type * = nullptr>
    constexpr /* implicit */
    optional_ref (Ptr&& ptr) noexcept
      : m_ptr (std::forward<Ptr> (ptr))
    { }

    /**
     * Constructor
     *
     * A converting constructor for types explicitly
     * convertible to `pointer`. This is so we don't
     * need to dereference or inspect pointers before
     * creation; we can just use the pointer directly.
     *
     * @tparam Ptr a type explicitly convertible to `pointer`.
     * @param ptr a pointer.
     */
    template <typename Ptr,
              typename std::enable_if<std::is_constructible<pointer, Ptr>::value
                                  &&! std::is_convertible<Ptr, pointer>::value>::type * = nullptr>
    constexpr explicit
    optional_ref (Ptr&& ptr) noexcept
      : m_ptr (std::forward<Ptr> (ptr))
    { }

    /**
     * Constructor
     *
     * A constructor for the case where `pointer`
     * is constructible from `U *`.
     *
     * @tparam U a referenced value type.
     * @param ref a reference where `pointer` is constructible from its pointer.
     */
    template <typename U,
              typename std::enable_if<constructible_from_pointer_to<U>::value>::type * = nullptr>
    constexpr explicit
    optional_ref (U& ref) noexcept
      : optional_ref (&ref)
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
     * A copy constructor from another optional_ref for the case
     * where `U *` is implicitly convertible to type `pointer`.
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
     * where `pointer` is explicitly constructible from `U *`.
     *
     * @tparam U a referenced value type.
     * @param ref an optional_ref which contains a pointer from
     *            which `pointer` may be explicitly constructed.
     */
    template <typename U,
              typename std::enable_if<std::is_constructible<pointer, U *>::value
                                  &&! std::is_convertible<U *, pointer>::value>::type * = nullptr>
    constexpr explicit
    optional_ref (const optional_ref<U>& other) noexcept
      : m_ptr (other.get_pointer ())
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
     * Never fails. The return is forwarded from `get_component_pointer`.
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
    value_or (const value_type&& default_value) const noexcept
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
              typename = typename std::enable_if<constructible_from_pointer_to<U>::value>::type>
    GCH_NODISCARD constexpr
    const_reference
    value_or (const U&& default_value) const noexcept = delete;

    /**
     * Swap the contained reference with that of `other`.
     *
     * @param other a reference to another `optional_ref`.
     */
    GCH_CPP14_CONSTEXPR
    void
    swap (optional_ref& other) noexcept
    {
      // manually done so we can lower the version requirements for constexpr
      pointer tmp = m_ptr;
      m_ptr       = other.m_ptr;
      other.m_ptr = tmp;
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
     * Internally, sets the pointer to the argument pointer.
     *
     * @tparam U a reference type where `pointer` is explicitly constructible from its pointer.
     * @param ref an lvalue reference.
     * @return the contained reference.
     */
    template <typename Ptr,
              typename = typename std::enable_if<std::is_constructible<pointer, Ptr>::value>::type>
    GCH_CPP14_CONSTEXPR
    reference
    emplace (Ptr&& ptr) noexcept
    {
      return *(m_ptr = pointer (ptr));
    }

    /**
     * Sets the contained reference.
     *
     * Internally, sets the a pointer to the address of the referenced value.
     *
     * @tparam U a pointer convertible to `pointer`.
     * @param ref an lvalue reference.
     * @return the contained reference.
     */
    template <typename U,
              typename = typename std::enable_if<constructible_from_pointer_to<U>::value>::type>
    GCH_CPP14_CONSTEXPR
    reference
    emplace (U& ref) noexcept
    {
      return emplace (&ref);
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
     * @return the contained reference.
     */
    template <typename U,
              typename = typename std::enable_if<constructible_from_pointer_to<U>::value>>
    GCH_CPP14_CONSTEXPR
    reference
    emplace (const optional_ref<U>& other) noexcept
    {
      return emplace (other.get_pointer ());
    }

    /**
     * Compares reference addresses.
     *
     * Does a pointer comparison between the argument and
     * the stored reference.
     *
     * @tparam U a reference type comparable to `pointer`.
     * @param ref an lvalue reference.
     * @return whether `*this` contains `ref`
     */
    template <typename U>
    GCH_NODISCARD constexpr
    bool
    contains (U& ref) const noexcept
    {
      return &ref == m_ptr;
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
    return l.has_value () == r.has_value ()
       &&  (! l.has_value () || l.get_pointer () == r.get_pointer ());
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
    return l.has_value () != r.has_value ()
       ||  (l.has_value () && l.get_pointer () != r.get_pointer ());
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
    return r.has_value ()
       &&  (! l.has_value () || l.get_pointer () < r.get_pointer ());
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
    return l.has_value ()
       &&  (! r.has_value () || l.get_pointer () > r.get_pointer ());
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
    return ! l.has_value ()
         ||  (r.has_value () && l.get_pointer () <= r.get_pointer ());
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
    return ! r.has_value ()
         ||  (l.has_value () && l.get_pointer () >= r.get_pointer ());
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
                                              : (l.has_value ()   <=> r.has_value ());
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
  constexpr
  std::strong_ordering
  operator<=> (const optional_ref<T>& l, nullopt_t) noexcept
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
  GCH_NODISCARD GCH_CPP20_CONSTEVAL
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
  GCH_NODISCARD GCH_CPP20_CONSTEVAL
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
  GCH_NODISCARD GCH_CPP20_CONSTEVAL
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
  GCH_NODISCARD GCH_CPP20_CONSTEVAL
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
    noexcept (noexcept (l.get_pointer () == r))
    -> decltype (l.get_pointer () == r)
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
    noexcept (noexcept (l == r.get_pointer ()))
    -> decltype (l == r.get_pointer ())
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
    noexcept (noexcept (l.get_pointer () != r))
    -> decltype (l.get_pointer () != r)
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
    noexcept (noexcept (l != r.get_pointer ()))
    -> decltype (l != r.get_pointer ())
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
    noexcept (noexcept (l.get_pointer () < r))
    -> decltype (l.get_pointer () < r)
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
    noexcept (noexcept (l < r.get_pointer ()))
    -> decltype (l < r.get_pointer ())
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
    noexcept (noexcept (l.get_pointer () > r))
    -> decltype (l.get_pointer () > r)
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
    noexcept (noexcept (l > r.get_pointer ()))
    -> decltype (l > r.get_pointer ())
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
    noexcept (noexcept (l.get_pointer () <= r))
    -> decltype (l.get_pointer () <= r)
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
    noexcept (noexcept (l <= r.get_pointer ()))
    -> decltype (l <= r.get_pointer ())
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
    noexcept (noexcept (l.get_pointer () >= r))
    -> decltype (l.get_pointer () >= r)
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
    noexcept (noexcept (l >= r.get_pointer ()))
    -> decltype (l >= r.get_pointer ())
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
  inline GCH_CPP14_CONSTEXPR
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
   * @tparam U a value type.
   * @param ref a reference.
   * @return an `optional_ref<U>` created from the argument.
   *
   * @see std::make_optional
   */
  template <typename U>
  GCH_NODISCARD constexpr
  optional_ref<U>
  make_optional_ref (U& ref) noexcept
  {
    return optional_ref<U> { ref };
  }

  /**
   * An optional_ref creation function.
   *
   * Creates an `optional_ref` with the specified argument.
   *
   * @tparam U a value type.
   * @param ref a pointer.
   * @return an `optional_ref<U>` created from the argument.
   *
   * @see std::make_optional
   */
  template <typename U>
  GCH_NODISCARD constexpr
  optional_ref<U>
  make_optional_ref (U *ptr) noexcept
  {
    return optional_ref<U> { ptr };
  }

#ifdef GCH_CTAD_SUPPORT

  template <typename U>
  optional_ref (U&) -> optional_ref<U>;

  template <typename U>
  optional_ref (U *) -> optional_ref<U>;

#endif

  /**
   * A convenience alias for const references.
   *
   * @tparam T an unqualified value type.
   */
  template <typename T>
  using optional_cref = optional_ref<const T>;

  namespace detail
  {

    template <typename T, typename Type, typename Base, typename ...Args>
    constexpr
    auto
    maybe_invoke_optional_ref (optional_ref<T> opt, Type Base::* f, Args&&... args)
      -> typename std::enable_if<std::is_member_function_pointer<decltype (f)>::value
                             &&  std::is_object<decltype (
                                   ((*opt).*f) (std::forward<Args> (args)...))>::value
                             &&  std::is_default_constructible<decltype (
                                   ((*opt).*f) (std::forward<Args> (args)...))>::value,
                                 decltype (((*opt).*f) (std::forward<Args> (args)...))>::type
    {
      using ret_type = decltype (((*opt).*f) (std::forward<Args> (args)...));
      return opt ? ((*opt).*f) (std::forward<Args> (args)...) : ret_type ();
    }

    template <typename T, typename Type, typename Base, typename ...Args>
    constexpr
    auto
    maybe_invoke_optional_ref (optional_ref<T> opt, Type Base::* f, Args&&... args)
      -> typename std::enable_if<std::is_member_function_pointer<decltype (f)>::value
                             &&! std::is_object<decltype (
                                   ((*opt).*f) (std::forward<Args> (args)...))>::value
                             &&  std::is_lvalue_reference<decltype (
                                   ((*opt).*f) (std::forward<Args> (args)...))>::value,
                                 optional_ref<typename std::remove_reference<decltype (
                                   ((*opt).*f) (std::forward<Args> (args)...))>::type>>::type
    {
      using ref = decltype (((*opt).*f) (std::forward<Args> (args)...));
      using ret_type = optional_ref<typename std::remove_reference<ref>::type>;
      return opt ? ret_type (((*opt).*f) (std::forward<Args> (args)...)) : ret_type ();
    }

    template <typename T, typename Type, typename Base, typename ...Args>
    inline GCH_CPP14_CONSTEXPR
    auto
    maybe_invoke_optional_ref (optional_ref<T> opt, Type Base::* f, Args&&... args)
      -> typename std::enable_if<std::is_member_function_pointer<decltype (f)>::value
                             &&! std::is_object<decltype (
                                   ((*opt).*f) (std::forward<Args> (args)...))>::value
                             &&! std::is_lvalue_reference<decltype (
                                   ((*opt).*f) (std::forward<Args> (args)...))>::value,
                                 void>::type
    {
      if (opt)
        ((*opt).*f) (std::forward<Args> (args)...);
    }

    template <typename T, typename Type, typename Base>
    constexpr
    auto
    maybe_invoke_optional_ref (optional_ref<T> opt, Type Base::* m)
      -> typename std::enable_if<std::is_member_object_pointer<decltype (m)>::value,
                                 optional_ref<typename std::remove_reference<decltype (
                                   (*opt).*m)>::type>>::type
    {
      using ref = decltype ((*opt).*m);
      using ret_type = optional_ref<typename std::remove_reference<ref>::type>;
      return opt ? ret_type ((*opt).*m) : ret_type ();
    }

    template <typename T, typename Functor, typename ...Args>
    constexpr
    auto
    maybe_invoke_optional_ref (optional_ref<T> opt, Functor&& f, Args&&... args)
      -> typename std::enable_if<
               std::is_object<decltype (
                 std::forward<Functor> (f) (*opt, std::forward<Args> (args)...))>::value
           &&  std::is_default_constructible<decltype (
                 std::forward<Functor> (f) (*opt, std::forward<Args> (args)...))>::value,
               decltype (
                 std::forward<Functor> (f) (*opt, std::forward<Args> (args)...))>::type
    {
      using ret_type = decltype (std::forward<Functor> (f) (*opt, std::forward<Args> (args)...));
      return opt ? std::forward<Functor> (f) (*opt, std::forward<Args> (args)...) : ret_type ();
    }

    template <typename T, typename Functor, typename ...Args>
    constexpr
    auto
    maybe_invoke_optional_ref (optional_ref<T> opt, Functor&& f, Args&&... args)
      -> typename std::enable_if<
             ! std::is_object<decltype (
                 std::forward<Functor> (f) (*opt, std::forward<Args> (args)...))>::value
           &&  std::is_lvalue_reference<decltype (
                 std::forward<Functor> (f) (*opt, std::forward<Args> (args)...))>::value,
               optional_ref<typename std::remove_reference<decltype (
                 std::forward<Functor> (f) (*opt, std::forward<Args> (args)...))>::type>>::type
    {
      using ref = decltype (std::forward<Functor> (f) (*opt, std::forward<Args> (args)...));
      using ret_type = optional_ref<typename std::remove_reference<ref>::type>;

      return opt ? ret_type (std::forward<Functor> (f) (*opt, std::forward<Args> (args)...))
                 : ret_type ();
    }

    template <typename T, typename Functor, typename ...Args>
    inline GCH_CPP14_CONSTEXPR
    auto
    maybe_invoke_optional_ref (optional_ref<T> opt, Functor&& f, Args&&... args)
      -> typename std::enable_if<
             ! std::is_object<decltype (
                 std::forward<Functor> (f) (*opt, std::forward<Args> (args)...))>::value
           &&! std::is_lvalue_reference<decltype (
                 std::forward<Functor> (f) (*opt, std::forward<Args> (args)...))>::value,
               void>::type
    {
      if (opt)
        std::forward<Functor> (f) (*opt, std::forward<Args> (args)...);
    }

    template <typename Void, typename Optional, typename Functor, typename ...Args>
    struct maybe_invoke_result_optional_ref
    { };

    template <typename T, typename Functor, typename ...Args>
    struct maybe_invoke_result_optional_ref<
      decltype (static_cast<void> (maybe_invoke_optional_ref (std::declval<optional_ref<T>> (),
                                                              std::declval<Functor> (),
                                                              std::declval<Args> ()...))),
      optional_ref<T>, Functor, Args...>
    {
      using type = decltype (maybe_invoke_optional_ref (std::declval<optional_ref<T>> (),
                                                        std::declval<Functor> (),
                                                        std::declval<Args> ()...));
    };

    template <typename Void, typename Optional, typename Functor, typename ...Args>
    struct is_maybe_invocable_optional_ref
      : std::false_type
    { };

    template <typename T, typename Functor, typename ...Args>
    struct is_maybe_invocable_optional_ref<
          decltype (static_cast<void> (maybe_invoke_optional_ref (std::declval<optional_ref<T>> (),
                                                                  std::declval<Functor> (),
                                                                  std::declval<Args> ()...))),
          optional_ref<T>, Functor, Args...>
      : std::true_type
    { };

  }

  template <typename Optional, typename Functor, typename ...Args>
  struct maybe_invoke_result;

  template <typename T, typename Functor, typename ...Args>
  struct maybe_invoke_result<optional_ref<T>, Functor, Args...>
    : detail::maybe_invoke_result_optional_ref<void, optional_ref<T>, Functor, Args...>
  { };

  template <typename T, typename Functor, typename ...Args>
  struct maybe_invoke_result<optional_ref<T>&, Functor, Args...>
    : maybe_invoke_result<optional_ref<T>, Functor, Args...>
  { };

  template <typename T, typename Functor, typename ...Args>
  struct maybe_invoke_result<const optional_ref<T>&, Functor, Args...>
    : maybe_invoke_result<optional_ref<T>, Functor, Args...>
  { };

  template <typename T, typename Functor, typename ...Args>
  struct maybe_invoke_result<optional_ref<T>&&, Functor, Args...>
    : maybe_invoke_result<optional_ref<T>, Functor, Args...>
  { };

  template <typename T, typename Functor, typename ...Args>
  struct maybe_invoke_result<const optional_ref<T>&&, Functor, Args...>
    : maybe_invoke_result<optional_ref<T>, Functor, Args...>
  { };

  template <typename Optional, typename Functor, typename ...Args>
  using maybe_invoke_result_t = typename maybe_invoke_result<Optional, Functor, Args...>::type;

  template <typename Optional, typename Functor, typename ...Args>
  struct is_maybe_invocable;

  template <typename T, typename Functor, typename ...Args>
  struct is_maybe_invocable<optional_ref<T>, Functor, Args...>
    : detail::is_maybe_invocable_optional_ref<void, optional_ref<T>, Functor, Args...>
  { };

  template <typename T, typename Functor, typename ...Args>
  struct is_maybe_invocable<optional_ref<T>&, Functor, Args...>
    : is_maybe_invocable<optional_ref<T>, Functor, Args...>
  { };

  template <typename T, typename Functor, typename ...Args>
  struct is_maybe_invocable<const optional_ref<T>&, Functor, Args...>
    : is_maybe_invocable<optional_ref<T>, Functor, Args...>
  { };

  template <typename T, typename Functor, typename ...Args>
  struct is_maybe_invocable<optional_ref<T>&&, Functor, Args...>
    : is_maybe_invocable<optional_ref<T>, Functor, Args...>
  { };

  template <typename T, typename Functor, typename ...Args>
  struct is_maybe_invocable<const optional_ref<T>&&, Functor, Args...>
    : is_maybe_invocable<optional_ref<T>, Functor, Args...>
  { };

#ifdef GCH_VARIABLE_TEMPLATES

  template <typename Optional, typename Functor, typename ...Args>
  GCH_INLINE_VAR constexpr
  bool
  is_maybe_invocable_v = is_maybe_invocable<Optional, Functor, Args...>::value;

#endif

  /**
   * Invokes a functor on the optional if and only if it has a value.
   *
   * On the return type:
   *   let `U = std::invoke_result_t<Functor, T&, Args...>`
   *   if `U` is
   *     -     object and     default constructible -> `U`
   *     -     object and not default constructible -> <ill-formed>
   *     - not object and     lvalue reference      -> `optional_ref<std::remove_reference_t<U>>`
   *     - not object and not lvalue reference      -> `void`
   *
   * In the case that U is an object and default constructible, the intent
   * is that U is either a `std::optional` or `optional_ref`, but holds no
   * restriction to those types so that user defined monads can be used.
   * In the aforementioned case, a default constructed value of `U` is
   * returned if `opt` has no value.
   *
   * The functor may be a free function pointer, a pointer to member function,
   * a pointer to member object, or a function object.
   *
   * @tparam T the value type of an `optional_ref`
   * @tparam Functor a functor
   * @tparam Args argument types passed to the function
   * @param opt an `optional_ref`
   * @param f a functor to be invoked
   * @param args arguments passed to the function
   * @return [see above]
   */
  template <typename T, typename Functor, typename ...Args>
  typename std::enable_if<is_maybe_invocable<optional_ref<T>, Functor, Args...>::value,
                          maybe_invoke_result_t<optional_ref<T>, Functor, Args...>>::type
  maybe_invoke (optional_ref<T> opt, Functor&& f, Args&&... args)
  {
    return detail::maybe_invoke_optional_ref (opt,
                                              std::forward<Functor> (f),
                                              std::forward<Args> (args)...);
  }

  /**
   * Calls maybe_invoke with the specified function.
   *
   * @tparam T the value type of an `optional_ref`
   * @tparam Functor a functor
   * @param opt an `optional_ref`
   * @param f a functor to be invoked
   * @return the result of the call to `maybe_invoke`
   *
   * @see gch::maybe_invoke
   */
  template <typename T, typename Functor,
            typename std::enable_if<
              is_maybe_invocable<optional_ref<T>, Functor>::value>::type * = nullptr>
  constexpr
  maybe_invoke_result_t<optional_ref<T>, Functor>
  operator>>= (optional_ref<T> opt, Functor&& f)
  {
    return maybe_invoke (opt, std::forward<Functor> (f));
  }

  template <typename T>
  constexpr
  maybe_invoke_result_t<optional_ref<T>, void (T&)>
  operator>>= (optional_ref<T> opt, void f (T&))
  {
    return maybe_invoke (opt, f);
  }

  template <typename T>
  constexpr
  maybe_invoke_result_t<optional_ref<T>, T (T&)>
  operator>>= (optional_ref<T> opt, T f (T&))
  {
    return maybe_invoke (opt, f);
  }

  template <typename T>
  constexpr
  maybe_invoke_result_t<optional_ref<T>, T& (T&)>
  operator>>= (optional_ref<T> opt, T& f (T&))
  {
    return maybe_invoke (opt, f);
  }

  template <typename T>
  constexpr
  maybe_invoke_result_t<optional_ref<T>, optional_ref<T> (T&)>
  operator>>= (optional_ref<T> opt, optional_ref<T> f (T&))
  {
    return maybe_invoke (opt, f);
  }

  // FIXME: Citing temp.deduct.type-(5.5.1), I think this is defined to be ill-formed for
  //        overloaded functions because of the deduction for Return performed by the second
  //        argument. However, using it like this does work on GCC and MSVC. The overloads
  //        defined above this are to allow for some overloads to exist without deduction.
  //        However, note that the pointer-to-member-function overloads seen below should be
  //        legal because the cvref qualification of the second argument restricts it to one
  //        overload before deduction is performed.
  template <typename T, typename Return,
            typename std::enable_if<
              is_maybe_invocable<optional_ref<T>, Return (T&)>::value>::type * = nullptr>
  constexpr
  maybe_invoke_result_t<optional_ref<T>, Return (T&)>
  operator>>= (optional_ref<T> opt, Return f (T&))
  {
    return maybe_invoke (opt, f);
  }

  template <typename T, typename Base, typename Return,
            typename std::enable_if<
                  is_maybe_invocable<optional_ref<T>, Return (Base::*) (void)>::value
              &&  std::is_base_of<Base, typename std::decay<T>::type>::value
              &&! std::is_const<T>::value>::type * = nullptr>
  constexpr
  maybe_invoke_result_t<optional_ref<T>, Return (Base::*) (void)>
  operator>>= (optional_ref<T> opt, Return (Base::* f) (void))
  {
    return maybe_invoke (opt, f);
  }

  template <typename T, typename Base, typename Return,
            typename std::enable_if<
                  is_maybe_invocable<optional_ref<T>, Return (Base::*) (void) &>::value
              &&  std::is_base_of<Base, typename std::decay<T>::type>::value
              &&! std::is_const<T>::value>::type * = nullptr>
  constexpr
  maybe_invoke_result_t<optional_ref<T>, Return (Base::*) (void) &>
  operator>>= (optional_ref<T> opt, Return (Base::* f) (void) &)
  {
    return maybe_invoke (opt, f);
  }

  template <typename T, typename Base, typename Return,
            typename std::enable_if<
                  is_maybe_invocable<optional_ref<T>, Return (Base::*) (void) const>::value
              &&  std::is_base_of<Base, typename std::decay<T>::type>::value
              &&  std::is_const<T>::value>::type * = nullptr>
  constexpr
  maybe_invoke_result_t<optional_ref<T>, Return (Base::*) (void) const>
  operator>>= (optional_ref<T> opt, Return (Base::* f) (void) const)
  {
    return maybe_invoke (opt, f);
  }

  template <typename T, typename Base, typename Return,
            typename std::enable_if<
                  is_maybe_invocable<optional_ref<T>, Return (Base::*) (void) const &>::value
              &&  std::is_base_of<Base, typename std::decay<T>::type>::value
              &&  std::is_const<T>::value>::type * = nullptr>
  constexpr
  maybe_invoke_result_t<optional_ref<T>, Return (Base::*) (void) const &>
  operator>>= (optional_ref<T> opt, Return (Base::* f) (void) const &)
  {
    return maybe_invoke (opt, f);
  }

  template <typename T, typename Functor,
    typename std::enable_if<
      is_maybe_invocable<
        optional_ref<T>, decltype (std::declval<Functor> () ()) (T&)>::value>::type * = nullptr>
  constexpr
  maybe_invoke_result_t<optional_ref<T>, decltype (std::declval<Functor> () ()) (T&)>
  operator>> (optional_ref<T> opt, Functor&& f)
  {
    // make sure that the return is forwarded and not copied
    using ret_type = decltype (std::forward<Functor> (f) ());
    return opt >>= [&f](T&) -> ret_type { return std::forward<Functor> (f) (); };
  }

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
     * We just use the same hash as the underlying pointer.
     *
     * @param opt_ref a reference to a value of type `gch::optional_ref`.
     * @return a hash of the argument.
     */
    std::size_t
    operator() (const gch::optional_ref<T>& opt_ref) const noexcept
    {
      return std::hash<typename gch::optional_ref<T>::pointer> { } (opt_ref.get_pointer ());
    }
  };

}

#endif
