/** optional_ref.hpp
 * Defines an optional reference.
 *
 * Copyright © 2019-2021 Gene Harvey
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

#ifndef GCH_CPP14_CONSTEXPR
#  if defined (__cpp_constexpr) && __cpp_constexpr >= 201304L
#    define GCH_CPP14_CONSTEXPR constexpr
#    ifndef GCH_HAS_CPP14_CONSTEXPR
#      define GCH_HAS_CPP14_CONSTEXPR
#    endif
#  else
#    define GCH_CPP14_CONSTEXPR
#  endif
#endif

#ifndef GCH_CPP17_CONSTEXPR
#  if defined (__cpp_constexpr) && __cpp_constexpr >= 201603L
#    define GCH_CPP17_CONSTEXPR constexpr
#    ifndef GCH_HAS_CPP17_CONSTEXPR
#      define GCH_HAS_CPP17_CONSTEXPR
#    endif
#  else
#    define GCH_CPP17_CONSTEXPR
#  endif
#endif

#ifndef GCH_CPP20_CONSTEXPR
#  if defined (__cpp_constexpr) && __cpp_constexpr >= 201907L
#    define GCH_CPP20_CONSTEXPR constexpr
#    ifndef GCH_HAS_CPP20_CONSTEXPR
#      define GCH_HAS_CPP20_CONSTEXPR
#    endif
#  else
#    define GCH_CPP20_CONSTEXPR
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

#ifndef GCH_INLINE_VARIABLE
#  if defined (__cpp_inline_variables) && __cpp_inline_variables >= 201606L
#    define GCH_INLINE_VARIABLE inline
#  else
#    define GCH_INLINE_VARIABLE
#  endif
#endif

#ifndef GCH_IMPLICIT_CONVERSION
#  if defined (__cpp_conditional_explicit) && __cpp_conditional_explicit >= 201806L
#    define GCH_IMPLICIT_CONVERSION explicit (false)
#  else
#    define GCH_IMPLICIT_CONVERSION /* implicit */
#  endif
#endif

#if defined (__cpp_deduction_guides) && __cpp_deduction_guides >= 201703L
#  ifndef GCH_CTAD_SUPPORT
#    define GCH_CTAD_SUPPORT
#  endif
#endif

#if defined (__cpp_impl_three_way_comparison) && __cpp_impl_three_way_comparison >= 201907L
#  ifndef GCH_IMPL_THREE_WAY_COMPARISON
#    define GCH_IMPL_THREE_WAY_COMPARISON
#  endif
#  if defined (__has_include) && __has_include (<compare>)
#    include <compare>
#    if defined (__cpp_lib_three_way_comparison) && __cpp_lib_three_way_comparison >= 201907L
#      ifndef GCH_LIB_THREE_WAY_COMPARISON
#        define GCH_LIB_THREE_WAY_COMPARISON
#      endif
#    endif
#  endif
#endif

#if defined (__cpp_variable_templates) && __cpp_variable_templates >= 201304L
#  ifndef GCH_VARIABLE_TEMPLATES
#    define GCH_VARIABLE_TEMPLATES
#  endif
#endif

#if defined (__cpp_noexcept_function_type) && __cpp_noexcept_function_type >= 201510L
#  ifndef GCH_TYPESYSTEM_NOEXCEPT
#    define GCH_TYPESYSTEM_NOEXCEPT
#  endif
#endif

#if defined (__cpp_fold_expressions) && __cpp_fold_expressions >= 201603L
#  ifndef GCH_FOLD_EXPRESSIONS
#    define GCH_FOLD_EXPRESSIONS
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
    /**
     * A tag variable used to create the object.
     */
    static constexpr struct create_tag { } create { };

    /**
     * A tagged constructor.
     */
    constexpr explicit
    nullopt_t (create_tag) noexcept
    { }
  };

  /**
   * The global inline variable instantiation for `nullopt_t`.
   */
  GCH_INLINE_VARIABLE constexpr
  nullopt_t
  nullopt { nullopt_t::create };

  /**
   * An exception class for cases of bad access
   * to an `optional_ref`.
   */
  class bad_optional_access
    : public std::exception
  {
  public:
    /**
     * A default constructor.
     */
    bad_optional_access            (void)                           = default;

    /**
     * A copy constructor.
     */
    bad_optional_access            (const bad_optional_access&)     = default;

    /**
     * A move constructor.
     */
    bad_optional_access            (bad_optional_access&&) noexcept = default;

    /**
     * A copy-assignment operator.
     *
     * @return a reference to the object.
     */
    bad_optional_access& operator= (const bad_optional_access&)     = default;

    /**
     * A move-assignment operator.
     *
     * @return a reference to the object.
     */
    bad_optional_access& operator= (bad_optional_access&&) noexcept = default;

    /**
     * A destructor.
     */
    ~bad_optional_access           (void) noexcept override         = default;

    /**
     * Returns an error string.
     *
     * @return the error string.
     */
    GCH_NODISCARD
    const char *
    what (void) const noexcept override
    {
      return "bad optional_ref access";
    }
  };

  /**
   * A utility type-trait for identifying `optional_ref`s.
   *
   * @tparam OptionalRef the type to be inspected.
   */
  template <typename OptionalRef>
  struct is_optional_ref
    : std::false_type
  { };

#ifdef GCH_VARIABLE_TEMPLATES

  /**
   * An inline variable alias for `is_optional_ref`.
   *
   * @tparam OptionalRef the type to be inspected.
   */
  template <typename OptionalRef>
  GCH_INLINE_VARIABLE constexpr
  bool
  is_optional_ref_v = is_optional_ref<OptionalRef>::value;

#endif

  /**
   * A reference wrapper which provides semantics similar to `std::optional`.
   *
   * @tparam ValueType the value type of the stored reference.
   */
  template <typename ValueType>
  class optional_ref
  {
  public:
    static_assert(! std::is_reference<ValueType>::value,
      "optional_ref expects a value type as a template argument, not a reference.");

    using value_type      = ValueType;         /*!< The value type of the stored reference */
    using reference       = ValueType&;        /*!< The reference type to be wrapped       */
    using pointer         = ValueType *;       /*!< The pointer type to the value type     */
    using const_reference = const ValueType&;  /*!< A constant reference to `ValueType`    */
    using const_pointer   = const ValueType *; /*!< A constant pointer to `ValueType`      */

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
    constexpr GCH_IMPLICIT_CONVERSION
    optional_ref (nullopt_t) noexcept
    { }

    /**
     * Constructor
     *
     * A converting constructor for types implicitly
     * convertible to `pointer`. This is so we don't
     * need to dereference or inspect pointers before
     * creation; we can just use the pointer directly.
     *
     * @tparam Ptr a type implicitly convertible to `pointer`.
     * @param ptr a pointer.
     */
    template <typename Ptr,
              typename std::enable_if<std::is_constructible<pointer, Ptr>::value
                                  &&  std::is_convertible<Ptr, pointer>::value>::type * = nullptr>
    constexpr GCH_IMPLICIT_CONVERSION
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
              typename std::enable_if<constructible_from_pointer_to<U>::value>::type * = nullptr>
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
    constexpr GCH_IMPLICIT_CONVERSION
    optional_ref (optional_ref<U> other) noexcept
      : m_ptr (other.get_pointer ())
    { }

    /**
     * Constructor
     *
     * A copy constructor from another optional_ref for the case
     * where `pointer` is explicitly constructible from `U *`.
     *
     * @tparam U a referenced value type.
     * @param other an optional_ref which contains a pointer from
     *              which `pointer` may be explicitly constructed.
     */
    template <typename U,
              typename std::enable_if<std::is_constructible<pointer, U *>::value
                                  &&! std::is_convertible<U *, pointer>::value>::type * = nullptr>
    constexpr explicit
    optional_ref (optional_ref<U> other) noexcept
      : m_ptr (other.get_pointer ())
    { }

    /**
     * Returns a pointer representation of the reference.
     *
     * The pointer is the internal representation of the reference.
     * This function never fails.
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
     * is an rvalue reference of type `value_type`. Note that
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
              typename std::enable_if<constructible_from_pointer_to<U>::value>::type * = nullptr>
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
      // manually implemented so we can lower the constexpr version requirements to c++14
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
     * @tparam Ptr a type explicitly convertible to `pointer`.
     * @param ptr a pointer.
     * @return the contained reference.
     */
    template <typename Ptr,
              typename std::enable_if<std::is_constructible<pointer, Ptr>::value>::type * = nullptr>
    GCH_CPP14_CONSTEXPR
    reference
    emplace (Ptr&& ptr) noexcept
    {
      return *(m_ptr = pointer (std::forward<Ptr> (ptr)));
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
              typename std::enable_if<constructible_from_pointer_to<U>::value>::type * = nullptr>
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
              typename std::enable_if<constructible_from_pointer_to<U>::value>::type * = nullptr>
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
              typename std::enable_if<constructible_from_pointer_to<U>::value>::type * = nullptr>
    GCH_CPP14_CONSTEXPR
    reference
    emplace (optional_ref<U> other) noexcept
    {
      return emplace (other.get_pointer ());
    }

    /**
     * Compares reference addresses.
     *
     * Does a pointer comparison between the argument and
     * the stored reference.
     *
     * @tparam U a reference type whose address is comparable to `pointer`.
     * @param ref an lvalue reference.
     * @return whether `*this` contains `ref`.
     */
    template <typename U,
              typename CommonPointer = typename std::common_type<
                pointer,
                decltype (&std::declval<U&> ())
              >::type>
    GCH_NODISCARD constexpr
    bool
    refers_to (U& ref) const noexcept
    {
      return CommonPointer (&ref) == CommonPointer (m_ptr);
    }

    /**
     * A deleted version for rvalue references.
     *
     * Using `refers_to` with an rvalue reference is obviously
     * a mistake, so we might as well delete it.
     */
    template <typename U>
    bool
    refers_to (const U&&) const noexcept = delete;

    /**
     * Compares the stored pointer with nullptr.
     *
     * Returns true iff the has_value () is false.
     *
     * @return if the `optional_ref` is empty.
     */
    GCH_NODISCARD constexpr
    bool
    equal_pointer (std::nullptr_t) const noexcept
    {
      return ! has_value ();
    }

    /**
     * Compares the stored pointer with another pointer.
     *
     * The equality comparison takes place after conversion to a common pointer type
     *
     * @tparam Ptr the type of a pointer to be compared.
     * @tparam CommonPointer the common type between the stored pointer and the input.
     * @param ptr a pointer to be compared.
     * @return the result of the equality comparison after conversion to CommonPointer.
     */
    template <typename Ptr,
              typename CommonPointer = typename std::common_type<pointer, Ptr>::type>
    GCH_NODISCARD constexpr
    bool
    equal_pointer (Ptr&& ptr) const noexcept
    {
      return CommonPointer (ptr) == CommonPointer (m_ptr);
    }

    /**
     * Compares the stored pointer with that of another `optional_ref`.
     *
     * @tparam U the `value_type` of another `optional_ref`.
     * @tparam CommonPointer the common type between the stored pointers.
     * @param other another `optional_ref`.
     * @return whether the stored pointers are equal.
     */
    template <typename U,
              typename CommonPointer = typename std::common_type<pointer, U *>::type>
    GCH_NODISCARD constexpr
    bool
    equal_pointer (optional_ref<U> other) const noexcept
    {
      return CommonPointer (other.get_pointer ()) == CommonPointer (m_ptr);
    }

  private:
    /**
     * A pointer to the value.
     *
     * We use a pointer representation instead of an actual
     * reference so we can have rebind semantics.
     */
    pointer m_ptr { nullptr };
  };

  static_assert (std::is_trivially_copyable<optional_ref<int>>::value,
                 "optional_ref should be trivially copyable");

  /**
   * A partial-specialization for `is_optional_ref` (true case).
   *
   * @tparam T the `value_type` of an `optional_ref`.
   */
  template <typename T>
  struct is_optional_ref<optional_ref<T>>
    : std::true_type
  { };

#ifdef GCH_LIB_THREE_WAY_COMPARISON

  namespace concepts
  {

    /**
     * A concept utility for identifying `optional_ref`s.
     *
     * This might be useful in template parameters like
     * ```
     * template <OptionalRef opt>
     * void
     * do_something (void);
     * ```
     *
     * @tparam T the `value_type` of an `optional_ref`.
     */
    template <typename T>
    concept OptionalRef = is_optional_ref<T>::value;

  }

#endif

  // TODO: make comparisons more SFINAE-friendly

  /**
   * An equality comparison function.
   *
   * Compared by value.
   *
   * @tparam T the value type of `lhs`.
   * @tparam U the value type of `rhs`.
   * @param lhs an `optional_ref`.
   * @param rhs an `optional_ref`.
   * @return the result of the equality comparison.
   *
   * @see std::optional::operator==
   */
  template <typename T, typename U>
  GCH_NODISCARD constexpr
  bool
  operator== (optional_ref<T> lhs, optional_ref<U> rhs)
    noexcept (noexcept (*lhs == *rhs))
  {
    return (lhs.has_value () == rhs.has_value ()) && (! lhs.has_value () || (*lhs == *rhs));
  }

  /**
   * An inequality comparison function.
   *
   * Compared by value.
   *
   * @tparam T the value type of `lhs`.
   * @tparam U the value type of `rhs`.
   * @param lhs an `optional_ref`.
   * @param rhs an `optional_ref`.
   * @return the result of the inequality comparison.
   *
   * @see std::optional::operator!=
   */
  template <typename T, typename U>
  GCH_NODISCARD constexpr
  bool
  operator!= (optional_ref<T> lhs, optional_ref<U> rhs)
    noexcept (noexcept (*lhs != *rhs))
  {
    return (lhs.has_value () != rhs.has_value ()) || (lhs.has_value () && (*lhs != *rhs));
  }

  /**
   * An less-than comparison function.
   *
   * Compared by value.
   *
   * @tparam T the value type of `lhs`.
   * @tparam U the value type of `rhs`.
   * @param lhs an `optional_ref`.
   * @param rhs an `optional_ref`.
   * @return the result of the less-than comparison.
   *
   * @see std::optional::operator<
   */
  template <typename T, typename U>
  GCH_NODISCARD constexpr
  bool
  operator< (optional_ref<T> lhs, optional_ref<U> rhs)
    noexcept (noexcept (*lhs < *rhs))
  {
    return rhs.has_value () && (! lhs.has_value () || (*lhs < *rhs));
  }

  /**
   * A greater-than-equal comparison function.
   *
   * Compared by value.
   *
   * @tparam T the value type of `lhs`.
   * @tparam U the value type of `rhs`.
   * @param lhs an `optional_ref`.
   * @param rhs an `optional_ref`.
   * @return the result of the greater-than-equal comparison.
   *
   * @see std::optional::operator>=
   */
  template <typename T, typename U>
  GCH_NODISCARD constexpr
  bool
  operator>= (optional_ref<T> lhs, optional_ref<U> rhs)
    noexcept (noexcept (*lhs >= *rhs))
  {
    return ! rhs.has_value () || (lhs.has_value () && (*lhs >= *rhs));
  }

  /**
   * A greater-than comparison function.
   *
   * Compared by value.
   *
   * @tparam T the value type of `lhs`.
   * @tparam U the value type of `rhs`.
   * @param lhs an `optional_ref`.
   * @param rhs an `optional_ref`.
   * @return the result of the greater-than comparison.
   *
   * @see std::optional::operator>
   */
  template <typename T, typename U>
  GCH_NODISCARD constexpr
  bool
  operator> (optional_ref<T> lhs, optional_ref<U> rhs)
    noexcept (noexcept (*lhs > *rhs))
  {
    return lhs.has_value () && (! rhs.has_value () || (*lhs > *rhs));
  }

  /**
   * A less-than-equal comparison function.
   *
   * Compared by value.
   *
   * @tparam T the value type of `lhs`.
   * @tparam U the value type of `rhs`.
   * @param lhs an `optional_ref`.
   * @param rhs an `optional_ref`.
   * @return the result of the less-than-equal comparison.
   *
   * @see std::optional::operator<=
   */
  template <typename T, typename U>
  GCH_NODISCARD constexpr
  bool
  operator<= (optional_ref<T> lhs, optional_ref<U> rhs)
    noexcept (noexcept (*lhs <= *rhs))
  {
    return ! lhs.has_value () || (rhs.has_value () && (*lhs <= *rhs));
  }

#ifdef GCH_LIB_THREE_WAY_COMPARISON

  /**
   * A three-way comparison function.
   *
   * Compared by value.
   *
   * @tparam T the value type of `lhs`.
   * @tparam U the value type of `rhs`.
   * @param lhs an `optional_ref`.
   * @param rhs an `optional_ref`.
   * @return the result of the three-way comparison.
   *
   * @see std::optional::operator<=>
   */
  template <typename T, std::three_way_comparable_with<T> U>
  GCH_NODISCARD constexpr
  std::compare_three_way_result_t<T, U>
  operator<=> (optional_ref<T> lhs, optional_ref<U> rhs)
    noexcept (noexcept (std::compare_three_way { } (*lhs, *rhs)))
  {
    return (lhs.has_value () && rhs.has_value ()) ? std::compare_three_way { } (*lhs, *rhs)
                                                  : (lhs.has_value () <=> rhs.has_value ());
  }

#endif

  /**
   * An equality comparison function.
   *
   * Compared by value.
   *
   * @tparam T the value type of `lhs`.
   * @param lhs an `optional_ref`.
   * @return the result of the equality comparison.
   *
   * @see std::optional::operator==
   */
  template <typename T>
  GCH_NODISCARD constexpr
  bool
  operator== (optional_ref<T> lhs, nullopt_t) noexcept
  {
    return ! lhs.has_value ();
  }

  /**
   * An equality comparison function.
   *
   * Compared by value.
   *
   * @tparam T the value type of `rhs`.
   * @param rhs an `optional_ref`.
   * @return the result of the equality comparison.
   *
   * @see std::optional::operator==
   */
  template <typename T>
  GCH_NODISCARD constexpr
  bool
  operator== (nullopt_t, optional_ref<T> rhs) noexcept
  {
    return ! rhs.has_value ();
  }

#ifdef GCH_LIB_THREE_WAY_COMPARISON

  /**
   * A three-way comparison function.
   *
   * Compared by value.
   *
   * @tparam T the value type of `rhs`.
   * @param lhs an `optional_ref`.
   * @return the result of the three-way comparison.
   *
   * @see std::optional::operator<=>
   */
  template <typename T>
  GCH_NODISCARD constexpr
  std::strong_ordering
  operator<=> (optional_ref<T> lhs, nullopt_t) noexcept
  {
    return lhs.has_value () <=> false;
  }

  template <typename T>
  GCH_NODISCARD constexpr
  std::strong_ordering
  operator<=> (nullopt_t, optional_ref<T> rhs) noexcept
  {
    return false <=> rhs.has_value ();
  }

#else

  /**
   * An inequality comparison function.
   *
   * Compared by value.
   *
   * @tparam T the value type of `lhs`.
   * @param lhs an `optional_ref`.
   * @return the result of the inequality comparison.
   *
   * @see std::optional::operator!=
   */
  template <typename T>
  GCH_NODISCARD constexpr
  bool
  operator!= (optional_ref<T> lhs, nullopt_t) noexcept
  {
    return lhs.has_value ();
  }

  /**
   * An inequality comparison function.
   *
   * Compared by value.
   *
   * @tparam T the value type of `rhs`.
   * @param rhs an `optional_ref`.
   * @return the result of the inequality comparison.
   *
   * @see std::optional::operator!=
   */
  template <typename T>
  GCH_NODISCARD constexpr
  bool
  operator!= (nullopt_t, optional_ref<T> rhs) noexcept
  {
    return rhs.has_value ();
  }

  /**
   * A less-than comparison function.
   *
   * Compared by value.
   *
   * @tparam T the value type of `lhs`.
   * @return the result of the less-than comparison.
   *
   * @see std::optional::operator<
   */
  template <typename T>
  GCH_NODISCARD GCH_CPP20_CONSTEVAL
  bool
  operator< (optional_ref<T>, nullopt_t) noexcept
  {
    return false;
  }

  /**
   * A less-than comparison function.
   *
   * Compared by value.
   *
   * @tparam T the value type of `rhs`.
   * @return the result of the less-than comparison.
   *
   * @see std::optional::operator<
   */
  template <typename T>
  GCH_NODISCARD constexpr
  bool
  operator< (nullopt_t, optional_ref<T> rhs) noexcept
  {
    return rhs.has_value ();
  }

  /**
   * A greater-than-equal comparison function.
   *
   * Compared by value.
   *
   * @tparam T the value type of `lhs`.
   * @return the result of the greater-than-equal comparison.
   *
   * @see std::optional::operator>=
   */
  template <typename T>
  GCH_NODISCARD GCH_CPP20_CONSTEVAL
  bool
  operator>= (optional_ref<T>, nullopt_t) noexcept
  {
    return true;
  }

  /**
   * A greater-than-equal comparison function.
   *
   * Compared by value.
   *
   * @tparam T the value type of `rhs`.
   * @param rhs an `optional_ref`.
   * @return the result of the greater-than-equal comparison.
   *
   * @see std::optional::operator>=
   */
  template <typename T>
  GCH_NODISCARD constexpr
  bool
  operator>= (nullopt_t, optional_ref<T> rhs) noexcept
  {
    return ! rhs.has_value ();
  }

  /**
   * A greater-than comparison function.
   *
   * Compared by value.
   *
   * @tparam T the value type of `lhs`.
   * @param lhs an `optional_ref`.
   * @return the result of the greater-than comparison.
   *
   * @see std::optional::operator>
   */
  template <typename T>
  GCH_NODISCARD constexpr
  bool
  operator> (optional_ref<T> lhs, nullopt_t) noexcept
  {
    return lhs.has_value ();
  }

  /**
   * A greater-than comparison function.
   *
   * Compared by value.
   *
   * @tparam T the value type of `rhs`.
   * @return the result of the greater-than comparison.
   *
   * @see std::optional::operator>
   */
  template <typename T>
  GCH_NODISCARD GCH_CPP20_CONSTEVAL
  bool
  operator> (nullopt_t, optional_ref<T>) noexcept
  {
    return false;
  }

  /**
   * A less-than-equal comparison function.
   *
   * Compared by value.
   *
   * @tparam T the value type of `lhs`.
   * @param lhs an `optional_ref`.
   * @return the result of the less-than-equal comparison.
   *
   * @see std::optional::operator<=
   */
  template <typename T>
  GCH_NODISCARD constexpr
  bool
  operator<= (optional_ref<T> lhs, nullopt_t) noexcept
  {
    return ! lhs.has_value ();
  }

  /**
   * A less-than-equal comparison function.
   *
   * Compared by value.
   *
   * @tparam T the value type of `rhs`.
   * @return the result of the less-than-equal comparison.
   *
   * @see std::optional::operator<=
   */
  template <typename T>
  GCH_NODISCARD GCH_CPP20_CONSTEVAL
  bool
  operator<= (nullopt_t, optional_ref<T>) noexcept
  {
    return true;
  }

#endif

  /**
   * An equality comparison function.
   *
   * Compared by value.
   *
   * @tparam T the value type of `lhs`.
   * @tparam U a value type which is comparable to `T *`.
   * @param lhs an `optional_ref`.
   * @param rhs a comparable pointer.
   * @return the result of the equality comparison.
   *
   * @see std::optional::operator==
   */
  template <typename T, typename U,
            typename std::enable_if<! is_optional_ref<U>::value>::type * = nullptr>
  GCH_NODISCARD constexpr
  bool
  operator== (optional_ref<T> lhs, const U& rhs)
    noexcept (noexcept (*lhs == rhs))
  {
    return lhs.has_value () && (*lhs == rhs);
  }

  /**
   * An equality comparison function.
   *
   * Compared by value.
   *
   * @tparam T the value type of `rhs`.
   * @tparam U a value type which is comparable to `T *`.
   * @param lhs a comparable pointer.
   * @param rhs an `optional_ref`.
   * @return the result of the equality comparison.
   *
   * @see std::optional::operator==
   */
  template <typename T, typename U>
  GCH_NODISCARD constexpr
  bool
  operator== (const U& lhs, optional_ref<T> rhs)
    noexcept (noexcept (lhs == *rhs))
  {
    return rhs.has_value () && (lhs == *rhs);
  }

  /**
   * An inequality comparison function.
   *
   * Compared by value.
   *
   * @tparam T the value type of `lhs`.
   * @tparam U a value type which is comparable to `T *`.
   * @param lhs an `optional_ref`.
   * @param rhs a comparable pointer.
   * @return the result of the inequality comparison.
   *
   * @see std::optional::operator!=
   */
  template <typename T, typename U>
  GCH_NODISCARD constexpr
  bool
  operator!= (optional_ref<T> lhs, const U& rhs)
    noexcept (noexcept (*lhs != rhs))
  {
    return ! lhs.has_value () || (*lhs != rhs);
  }

  /**
   * An inequality comparison function.
   *
   * Compared by value.
   *
   * @tparam T the value type of `rhs`.
   * @tparam U a value type which is comparable to `T *`.
   * @param lhs a comparable pointer.
   * @param rhs an `optional_ref`.
   * @return the result of the inequality comparison.
   *
   * @see std::optional::operator!=
   */
  template <typename T, typename U>
  GCH_NODISCARD constexpr
  bool
  operator!= (const U& lhs, optional_ref<T> rhs)
    noexcept (noexcept (lhs != *rhs))
  {
    return ! rhs.has_value () || (lhs != *rhs);
  }

  /**
   * A less-than comparison function.
   *
   * Compared by value.
   *
   * @tparam T the value type of `lhs`.
   * @tparam U a value type which is comparable to `T *`.
   * @param lhs an `optional_ref`.
   * @param rhs a comparable pointer.
   * @return the result of the less-than comparison.
   *
   * @see std::optional::operator<
   */
  template <typename T, typename U>
  GCH_NODISCARD constexpr
  bool
  operator< (optional_ref<T> lhs, const U& rhs)
    noexcept (noexcept (*lhs < rhs))
  {
    return ! lhs.has_value () || (*lhs < rhs);
  }

  /**
   * A less-than comparison function.
   *
   * Compared by value.
   *
   * @tparam T the value type of `rhs`.
   * @tparam U a value type which is comparable to `T *`.
   * @param lhs a comparable pointer.
   * @param rhs an `optional_ref`.
   * @return the result of the less-than comparison.
   *
   * @see std::optional::operator<
   */
  template <typename T, typename U>
  GCH_NODISCARD constexpr
  bool
  operator< (const U& lhs, optional_ref<T> rhs)
  noexcept (noexcept (lhs < *rhs))
  {
    return rhs.has_value () && (lhs < *rhs);
  }

  /**
   * A greater-than-equal comparison function.
   *
   * Compared by value.
   *
   * @tparam T the value type of `lhs`.
   * @tparam U a value type which is comparable to `T *`.
   * @param lhs an `optional_ref`.
   * @param rhs a comparable pointer.
   * @return the result of the greater-than-equal comparison.
   *
   * @see std::optional::operator>=
   */
  template <typename T, typename U>
  GCH_NODISCARD constexpr
  bool
  operator>= (optional_ref<T> lhs, const U& rhs)
    noexcept (noexcept (*lhs >= rhs))
  {
    return lhs.has_value () && (*lhs >= rhs);
  }

  /**
   * A greater-than-equal comparison function.
   *
   * Compared by value.
   *
   * @tparam T the value type of `rhs`.
   * @tparam U a value type which is comparable to `T *`.
   * @param lhs a comparable pointer.
   * @param rhs an `optional_ref`.
   * @return the result of the greater-than-equal comparison.
   *
   * @see std::optional::operator>=
   */
  template <typename T, typename U>
  GCH_NODISCARD constexpr
  bool
  operator>= (const U& lhs, optional_ref<T> rhs)
    noexcept (noexcept (lhs >= *rhs))
  {
    return ! rhs.has_value () || (lhs >= *rhs);
  }

  /**
   * A greater-than comparison function.
   *
   * Compared by value.
   *
   * @tparam T the value type of `lhs`.
   * @tparam U a value type which is comparable to `T *`.
   * @param lhs an `optional_ref`.
   * @param rhs a comparable pointer.
   * @return the result of the greater-than comparison.
   *
   * @see std::optional::operator>
   */
  template <typename T, typename U>
  GCH_NODISCARD constexpr
  bool
  operator> (optional_ref<T> lhs, const U& rhs)
    noexcept (noexcept (*lhs > rhs))
  {
    return lhs.has_value () && (*lhs > rhs);
  }

  /**
   * A greater-than comparison function.
   *
   * Compared by value.
   *
   * @tparam T the value type of `rhs`.
   * @tparam U a value type which is comparable to `T *`.
   * @param lhs a comparable pointer.
   * @param rhs an `optional_ref`.
   * @return the result of the greater-than comparison.
   *
   * @see std::optional::operator>
   */
  template <typename T, typename U>
  GCH_NODISCARD constexpr
  bool
  operator> (const U& lhs, optional_ref<T> rhs)
    noexcept (noexcept (lhs > *rhs))
  {
    return ! rhs.has_value () || (lhs > *rhs);
  }

  /**
   * A less-than-equal comparison function.
   *
   * Compared by value.
   *
   * @tparam T the value type of `lhs`.
   * @tparam U a value type which is comparable to `T *`.
   * @param lhs an `optional_ref`.
   * @param rhs a comparable pointer.
   * @return the result of the less-than-equal comparison.
   *
   * @see std::optional::operator<=
   */
  template <typename T, typename U>
  GCH_NODISCARD constexpr
  bool
  operator<= (optional_ref<T> lhs, const U& rhs)
    noexcept (noexcept (*lhs <= rhs))
  {
    return ! lhs.has_value () || (*lhs <= rhs);
  }

  /**
   * A less-than-equal comparison function.
   *
   * Compared by value.
   *
   * @tparam T the value type of `rhs`.
   * @tparam U a value type which is comparable to `T *`.
   * @param lhs a comparable pointer.
   * @param rhs an `optional_ref`.
   * @return the result of the less-than-equal comparison.
   *
   * @see std::optional::operator<=
   */
  template <typename T, typename U>
  GCH_NODISCARD constexpr
  bool
  operator<= (const U& lhs, optional_ref<T> rhs)
    noexcept (noexcept (lhs <= *rhs))
  {
    return rhs.has_value () && (lhs <= *rhs);
  }

#ifdef GCH_LIB_THREE_WAY_COMPARISON

  /**
   * A three-way comparison function.
   *
   * Compared by value.
   *
   * @tparam T the value type of `lhs`.
   * @tparam U a value type which is comparable to `T *`.
   * @param lhs an `optional_ref`.
   * @param rhs a comparable pointer.
   * @return the result of the three-way comparison.
   *
   * @see std::optional::operator<=>
   */
  template <typename T, typename U>
  requires (! is_optional_ref<U>::value && std::three_way_comparable_with<T, U>)
  GCH_NODISCARD constexpr
  std::compare_three_way_result_t<T, U>
  operator<=> (optional_ref<T> lhs, const U& rhs)
    noexcept (noexcept (std::compare_three_way { } (*lhs, rhs)))
  {
    return lhs.has_value () ? std::compare_three_way { } (*lhs, rhs)
                            : std::strong_ordering::less;
  }

#endif

  /**
   * A swap function.
   *
   * Swaps the two `optional_ref`s of the same type.
   *
   * @tparam T the value type pointed to by the `optional_ref`s
   * @param lhs an `optional_ref`.
   * @param rhs an `optional_ref`.
   */
  template <typename T>
  inline GCH_CPP14_CONSTEXPR
  void
  swap (optional_ref<T>& lhs, optional_ref<T>& rhs) noexcept
  {
    lhs.swap (rhs);
  }

  /**
   * Compares the stored pointers of two `optional_ref`s.
   *
   * @tparam T the value type of the left-hand side `optional_ref`.
   * @tparam U the value type of the right-hand side `optional_ref`.
   * @param lhs an `optional_ref`.
   * @param rhs an `optional_ref`.
   * @return the result of the comparison of the stored pointers.
   */
  template <typename T, typename U>
  constexpr
  bool
  equal_pointer (optional_ref<T> lhs, optional_ref<U> rhs) noexcept
  {
    return lhs.equal_pointer (rhs);
  }

#ifdef GCH_FOLD_EXPRESSIONS

  /**
   * Compares the stored pointers of multiple `optional_ref`s.
   *
   * @tparam T the value type of an `optional_ref`.
   * @tparam Us a type pack containing pointer types or the value types of `optional_ref`s.
   * @param lhs an `optional_ref`.
   * @param rhs a parameter pack containing multiple `optional_ref`s.
   * @return whether all stored pointers are equal.
   */
  template <typename T, typename ...Us>
  constexpr
  bool
  equal_pointer (optional_ref<T> lhs, Us&&... rhs) noexcept
  {
    return (equal_pointer (lhs, std::forward<Us> (rhs)) && ...);
  }

#else

  template <typename T, typename U, typename ...Rest>
  constexpr
  bool
  equal_pointer (optional_ref<T> lhs, optional_ref<U> rhs, Rest&&... rest) noexcept
  {
    return equal_pointer (lhs, rhs) && equal_pointer (lhs, std::forward<Rest> (rest)...);
  }

#endif

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
   * @param ptr a pointer.
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
      noexcept (noexcept (((*opt).*f) (std::forward<Args> (args)...)))
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
      noexcept (noexcept (((*opt).*f) (std::forward<Args> (args)...)))
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
      noexcept (noexcept (((*opt).*f) (std::forward<Args> (args)...)))
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
      noexcept (noexcept ((*opt).*m))
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
      noexcept (noexcept (std::forward<Functor> (f) (*opt, std::forward<Args> (args)...)))
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
      noexcept (noexcept (std::forward<Functor> (f) (*opt, std::forward<Args> (args)...)))
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
      noexcept (noexcept (std::forward<Functor> (f) (*opt, std::forward<Args> (args)...)))
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
    {
      using nothrow = std::false_type;
    };

    template <typename T, typename Functor, typename ...Args>
    struct is_maybe_invocable_optional_ref<
          decltype (static_cast<void> (maybe_invoke_optional_ref (std::declval<optional_ref<T>> (),
                                                                  std::declval<Functor> (),
                                                                  std::declval<Args> ()...))),
          optional_ref<T>, Functor, Args...>
      : std::true_type
    {
      using nothrow = std::integral_constant<bool,
                        noexcept (maybe_invoke_optional_ref (std::declval<optional_ref<T>> (),
                                                             std::declval<Functor> (),
                                                             std::declval<Args> ()...))>;
    };

    template <typename Result, typename Enable = void>
    struct is_valid_maybe_invoke_result_impl
      : std::false_type
    { };

    template <typename Result>
    struct is_valid_maybe_invoke_result_impl<
          Result,
          typename std::enable_if<! std::is_object<Result>::value
                                ||  std::is_default_constructible<Result>::value>::type>
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

  template <typename Optional, typename Functor, typename ...Args>
  struct is_nothrow_maybe_invocable;

  template <typename T, typename Functor, typename ...Args>
  struct is_nothrow_maybe_invocable<optional_ref<T>, Functor, Args...>
    : detail::is_maybe_invocable_optional_ref<void, optional_ref<T>, Functor, Args...>::nothrow
  { };

  template <typename T, typename Functor, typename ...Args>
  struct is_nothrow_maybe_invocable<optional_ref<T>&, Functor, Args...>
    : is_nothrow_maybe_invocable<optional_ref<T>, Functor, Args...>
  { };

  template <typename T, typename Functor, typename ...Args>
  struct is_nothrow_maybe_invocable<const optional_ref<T>&, Functor, Args...>
    : is_nothrow_maybe_invocable<optional_ref<T>, Functor, Args...>
  { };

  template <typename T, typename Functor, typename ...Args>
  struct is_nothrow_maybe_invocable<optional_ref<T>&&, Functor, Args...>
    : is_nothrow_maybe_invocable<optional_ref<T>, Functor, Args...>
  { };

  template <typename T, typename Functor, typename ...Args>
  struct is_nothrow_maybe_invocable<const optional_ref<T>&&, Functor, Args...>
    : is_nothrow_maybe_invocable<optional_ref<T>, Functor, Args...>
  { };

  template <typename Result>
  struct is_valid_maybe_invoke_result
    : detail::is_valid_maybe_invoke_result_impl<Result>
  { };

#ifdef GCH_VARIABLE_TEMPLATES

  template <typename Optional, typename Functor, typename ...Args>
  GCH_INLINE_VARIABLE constexpr
  bool
  is_maybe_invocable_v = is_maybe_invocable<Optional, Functor, Args...>::value;

  template <typename Optional, typename Functor, typename ...Args>
  GCH_INLINE_VARIABLE constexpr
  bool
  is_nothrow_maybe_invocable_v = is_nothrow_maybe_invocable<Optional, Functor, Args...>::value;

  template <typename Result>
  GCH_INLINE_VARIABLE constexpr
  bool
  is_valid_maybe_invoke_result_v = is_valid_maybe_invoke_result<Result>::value;

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
  template <typename T, typename Functor, typename ...Args,
            typename std::enable_if<
              is_maybe_invocable<optional_ref<T>, Functor, Args...>::value>::type * = nullptr>
  constexpr
  maybe_invoke_result_t<optional_ref<T>, Functor, Args...>
  maybe_invoke (optional_ref<T> opt, Functor&& f, Args&&... args)
    noexcept (is_nothrow_maybe_invocable<optional_ref<T>, Functor, Args...>::value)
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
    noexcept (is_nothrow_maybe_invocable<optional_ref<T>, Functor>::value)
  {
    return maybe_invoke (opt, std::forward<Functor> (f));
  }

  // NOTE: The following overloads are a byproduct of the comment below.

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
  //        argument. However, using it like this does work on GCC and MSVC (but not on Clang).
  //
  //        The overloads defined above this are to allow for some overloads to exist without
  //        deduction of the return type.
  //
  //        Note that the pointer-to-member-function overloads seen below should be legal because
  //        the cvref qualification of the second argument restricts it to one overload before
  //        deduction of the return type is performed.
  template <typename T, typename Return,
            typename std::enable_if<
              is_maybe_invocable<optional_ref<T>,
                                 Return (T&)>::value>::type * = nullptr>
  constexpr
  maybe_invoke_result_t<optional_ref<T>, Return (T&)>
  operator>>= (optional_ref<T> opt, Return f (T&))
  {
    return maybe_invoke (opt, f);
  }

  template <typename T, typename Base, typename Return,
            typename std::enable_if<
                  is_maybe_invocable<optional_ref<T>, Return (Base::*) (void)>::value
              &&  std::is_base_of<Base, T>::value>::type * = nullptr>
  constexpr
  maybe_invoke_result_t<optional_ref<T>, Return (Base::*) (void)>
  operator>>= (optional_ref<T> opt, Return (Base::* f) (void))
  {
    return maybe_invoke (opt, f);
  }

  template <typename T, typename Base, typename Return,
            typename std::enable_if<
                  is_maybe_invocable<optional_ref<T>, Return (Base::*) (void) &>::value
              &&  std::is_base_of<Base, T>::value>::type * = nullptr>
  constexpr
  maybe_invoke_result_t<optional_ref<T>, Return (Base::*) (void) &>
  operator>>= (optional_ref<T> opt, Return (Base::* f) (void) &)
  {
    return maybe_invoke (opt, f);
  }

  template <typename T, typename Base, typename Return,
            typename std::enable_if<
                  is_maybe_invocable<optional_ref<const T>, Return (Base::*) (void) const>::value
              &&  std::is_base_of<Base, T>::value>::type * = nullptr>
  constexpr
  maybe_invoke_result_t<optional_ref<const T>, Return (Base::*) (void) const>
  operator>>= (optional_ref<const T> opt, Return (Base::* f) (void) const)
  {
    return maybe_invoke (opt, f);
  }

  template <typename T, typename Base, typename Return,
            typename std::enable_if<
                  is_maybe_invocable<optional_ref<const T>, Return (Base::*) (void) const &>::value
              &&  std::is_base_of<Base, T>::value>::type * = nullptr>
  constexpr
  maybe_invoke_result_t<optional_ref<const T>, Return (Base::*) (void) const &>
  operator>>= (optional_ref<const T> opt, Return (Base::* f) (void) const &)
  {
    return maybe_invoke (opt, f);
  }

#ifdef GCH_TYPESYSTEM_NOEXCEPT

  template <typename T>
  constexpr
  maybe_invoke_result_t<optional_ref<T>, void (T&) noexcept>
  operator>>= (optional_ref<T> opt, void f (T&) noexcept) noexcept
  {
    return maybe_invoke (opt, f);
  }

  template <typename T>
  constexpr
  maybe_invoke_result_t<optional_ref<T>, T (T&) noexcept>
  operator>>= (optional_ref<T> opt, T f (T&) noexcept) noexcept
  {
    return maybe_invoke (opt, f);
  }

  template <typename T>
  constexpr
  maybe_invoke_result_t<optional_ref<T>, T& (T&) noexcept>
  operator>>= (optional_ref<T> opt, T& f (T&) noexcept) noexcept
  {
    return maybe_invoke (opt, f);
  }

  template <typename T>
  constexpr
  maybe_invoke_result_t<optional_ref<T>, optional_ref<T> (T&) noexcept>
  operator>>= (optional_ref<T> opt, optional_ref<T> f (T&) noexcept) noexcept
  {
    return maybe_invoke (opt, f);
  }

  template <typename T, typename Return,
            typename std::enable_if<
              is_maybe_invocable<optional_ref<T>,
                                 Return (T&) noexcept>::value>::type * = nullptr>
  constexpr
  maybe_invoke_result_t<optional_ref<T>, Return (T&) noexcept>
  operator>>= (optional_ref<T> opt, Return f (T&) noexcept) noexcept
  {
    return maybe_invoke (opt, f);
  }

  template <typename T, typename Base, typename Return,
            typename std::enable_if<
                  is_maybe_invocable<optional_ref<T>,
                                     Return (Base::*) (void) noexcept>::value
              &&  std::is_base_of<Base, typename std::decay<T>::type>::value
              &&! std::is_const<T>::value>::type * = nullptr>
  constexpr
  maybe_invoke_result_t<optional_ref<T>, Return (Base::*) (void) noexcept>
  operator>>= (optional_ref<T> opt, Return (Base::* f) (void) noexcept) noexcept
  {
    return maybe_invoke (opt, f);
  }

  template <typename T, typename Base, typename Return,
            typename std::enable_if<
                  is_maybe_invocable<optional_ref<T>,
                                     Return (Base::*) (void) & noexcept>::value
              &&  std::is_base_of<Base, typename std::decay<T>::type>::value
              &&! std::is_const<T>::value>::type * = nullptr>
  constexpr
  maybe_invoke_result_t<optional_ref<T>, Return (Base::*) (void) & noexcept>
  operator>>= (optional_ref<T> opt, Return (Base::* f) (void) & noexcept) noexcept
  {
    return maybe_invoke (opt, f);
  }

  template <typename T, typename Base, typename Return,
            typename std::enable_if<
                  is_maybe_invocable<optional_ref<T>,
                                     Return (Base::*) (void) const noexcept>::value
              &&  std::is_base_of<Base, typename std::decay<T>::type>::value
              &&  std::is_const<T>::value>::type * = nullptr>
  constexpr
  maybe_invoke_result_t<optional_ref<T>, Return (Base::*) (void) const noexcept>
  operator>>= (optional_ref<T> opt, Return (Base::* f) (void) const noexcept) noexcept
  {
    return maybe_invoke (opt, f);
  }

  template <typename T, typename Base, typename Return,
            typename std::enable_if<
                  is_maybe_invocable<optional_ref<T>,
                                     Return (Base::*) (void) const & noexcept>::value
              &&  std::is_base_of<Base, typename std::decay<T>::type>::value
              &&  std::is_const<T>::value>::type * = nullptr>
  constexpr
  maybe_invoke_result_t<optional_ref<T>, Return (Base::*) (void) const & noexcept>
  operator>>= (optional_ref<T> opt, Return (Base::* f) (void) const & noexcept) noexcept
  {
    return maybe_invoke (opt, f);
  }

#endif

  template <typename T, typename Functor,
            typename std::enable_if<
              is_valid_maybe_invoke_result<decltype (std::declval<Functor> () ())>::value
            >::type * = nullptr>
  constexpr
  maybe_invoke_result_t<optional_ref<T>, decltype (std::declval<Functor> () ()) (T&)>
  operator>> (optional_ref<T> opt, Functor&& f)
    noexcept (noexcept (std::forward<Functor> (f) ()))
  {
    // make sure that the return is forwarded and not copied
    using ret_type = decltype (std::forward<Functor> (f) ());
    return opt >>= [&f](T&) noexcept (noexcept (std::forward<Functor> (f) ())) -> ret_type
                   { return std::forward<Functor> (f) (); };
  }

  /**
   * Performs a `dynamic_cast` on the pointer and converts to an `optional_ref`.
   *
   * Note: There is no use case of `U = S *` because `S *` cannot
   *       be a polymorphic object, so these pointer cases are fine.
   *
   * @tparam T the target value type of the `dynamic_cast`.
   * @tparam U the value type of the input pointer
   * @param ptr a polymorphic pointer
   * @return the result of the `dynamic_cast` wrapped into an `optional_ref`.
   */
  template <typename T, typename U>
  inline
  optional_ref<T>
  maybe_cast (U *ptr) noexcept
  {
    return optional_ref<T> { dynamic_cast<T *> (ptr) };
  }

  /**
   * Performs a `dynamic_cast` on the pointer and converts to an `optional_ref`.
   *
   * Note: const-qualification is not necessary for `T`.
   *
   * @tparam T the target value type of the `dynamic_cast`.
   * @tparam U the value type of the input pointer
   * @param ptr a polymorphic pointer
   * @return the result of the `dynamic_cast` wrapped into an `optional_ref`.
   */
  template <typename T, typename U>
  inline
  optional_ref<const T>
  maybe_cast (const U *ptr) noexcept
  {
    return optional_ref<const T> { dynamic_cast<const T *> (ptr) };
  }

  /**
   * Performs a `dynamic_cast` on the reference and converts to an `optional_ref`.
   *
   * @tparam T the target value type of the `dynamic_cast`.
   * @tparam U the value type of the input pointer
   * @param ref a polymorphic reference
   * @return the result of the `dynamic_cast` wrapped into an `optional_ref`.
   */
  template <typename T, typename U>
  inline
  optional_ref<T>
  maybe_cast (U& ref) noexcept
  {
    return maybe_cast<T> (&ref);
  }

  /**
   * Performs a `dynamic_cast` on the reference and converts to an `optional_ref`.
   *
   * Note: const-qualification is not necessary for `T`.
   *
   * @tparam T the target value type of the `dynamic_cast`.
   * @tparam U the value type of the input pointer
   * @param ref a polymorphic reference
   * @return the result of the `dynamic_cast` wrapped into an `optional_ref`.
   */
  template <typename T, typename U>
  inline
  optional_ref<const T>
  maybe_cast (const U& ref) noexcept
  {
    return maybe_cast<T> (&ref);
  }

  /**
   * A deleted version for rvalue-references (since
   * `optional_ref`s cannot hold rvalue-references).
   */
  template <typename T, typename U>
  void
  maybe_cast (const U&&) = delete;

  /**
   * Forwards the pointer held by an `optional_ref` to `maybe_cast`.
   *
   * @tparam T the target value type of the `dynamic_cast`.
   * @tparam U the value type of the input pointer
   * @param opt an `optional_ref` which refers to a polymorphic type.
   * @return the result of a `maybe_cast` performed on the pointer stored by `opt`.
   */
  template <typename T, typename U>
  inline
  optional_ref<T>
  maybe_cast (optional_ref<U> opt) noexcept
  {
    return maybe_cast<T> (opt.get_pointer ());
  }

  /**
   * Forwards the pointer held by an `optional_ref` to `maybe_cast`.
   *
   * Note: const-qualification is not necessary for `T`.
   *
   * @tparam T the target value type of the `dynamic_cast`.
   * @tparam U the value type of the input pointer
   * @param opt an `optional_ref` which refers to a const polymorphic type.
   * @return the result of a `maybe_cast` performed on the pointer stored by `opt`.
   */
  template <typename T, typename U>
  inline
  optional_ref<const T>
  maybe_cast (optional_ref<const U> opt) noexcept
  {
    return maybe_cast<T> (opt.get_pointer ());
  }

  template <typename T>
  constexpr
  optional_ref<T>
  as_mutable (optional_ref<const T> opt) noexcept
  {
    return optional_ref<T> { const_cast<T *> (opt.get_pointer ()) };
  }

} // namespace gch

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

} // namespace std

#endif
