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

#include <memory>
#include <type_traits>
#include <typeindex>
#include <utility>

#if __has_cpp_attribute(nodiscard) >= 201603L
#  define GCH_NODISCARD [[nodiscard]]
#else
#  define GCH_NODISCARD
#endif

#if __cpp_lib_addressof_constexpr >= 201603L
#  define GCH_CONSTEXPR_ADDRESSOF constexpr
#else
#  define GCH_CONSTEXPR_ADDRESSOF
#endif

#if __cpp_inline_variables >= 201606
#  define GCH_INLINE_VARS inline
#else
#  define GCH_INLINE_VARS
#endif

#if __cpp_lib_constexpr_algorithms >= 201806L
#  define GCH_CONSTEXPR_SWAP constexpr
#else
#  define GCH_CONSTEXPR_SWAP
#endif

#if __cpp_constexpr >= 201304L
#  define GCH_CPP14_CONSTEXPR constexpr
#else 
#  define GCH_CPP14_CONSTEXPR
#endif

#if __cpp_deduction_guides >= 201703
#  define GCH_DEDUCTION_GUIDE_SUPPORT
#endif

namespace gch
{
  /**
   * A struct that provides semantics for
   * empty `optional_ref`s.
   */
  struct nullopt_t
  {
    static constexpr struct engage_tag { } engage { };
    constexpr explicit nullopt_t (engage_tag) noexcept { }
  };

  GCH_INLINE_VARS constexpr nullopt_t nullopt { nullopt_t::engage };
  
  /**
   * An exception class for cases of bad access
   * to an `optional_ref`.
   */
  class bad_optional_access : public std::exception
  {
  public:
    GCH_NODISCARD
    const char* what (void) const noexcept override { return "bad optional_ref access"; }
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
    using constructible_from = std::is_constructible<pointer, U*>;
  
    template <typename U>
    using convertible_from = std::is_convertible<U*, pointer>;
    
    template <typename>
    struct is_optional_ref : std::false_type { };
    
    template <typename T>
    struct is_optional_ref<optional_ref<T>> : std::true_type { };
    
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
    constexpr /* implicit */ optional_ref (nullopt_t) noexcept { };
  
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
              typename = typename std::enable_if<constructible_from<U>::value>::type,
              typename std::enable_if<convertible_from<U>::value, bool>::type = true>
    GCH_CONSTEXPR_ADDRESSOF /* implicit */ optional_ref (U& ref) noexcept
      : m_ptr (std::addressof (ref))
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
              typename = typename std::enable_if<constructible_from<U>::value>::type,
              typename std::enable_if<! convertible_from<U>::value, bool>::type = false>
    GCH_CONSTEXPR_ADDRESSOF explicit optional_ref (U& ref) noexcept
      : m_ptr (std::addressof (static_cast<reference> (ref)))
    { }
  
    /**
     * Constructor
     * 
     * A deleted contructor for the case where `ref` is an rvalue reference.
     */
    template <typename U,
              typename = typename std::enable_if<! is_optional_ref<U>::value>::type>
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
      typename = typename std::enable_if<std::is_constructible<pointer, U&&>::value>::type,
      typename std::enable_if<std::is_convertible<U&&, pointer>::value, bool>::type = true>
    constexpr /* implicit */ optional_ref (U&& ptr) noexcept
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
      typename = typename std::enable_if<std::is_constructible<pointer, U&&>::value>::type,
      typename std::enable_if<! std::is_convertible<U&&, pointer>::value, bool>::type = false>
    constexpr explicit optional_ref (U&& ptr) noexcept
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
              typename = typename std::enable_if<constructible_from<U>::value>::type,
              typename std::enable_if<convertible_from<U>::value, bool>::type = true>
    constexpr /* implicit */ optional_ref (const optional_ref<U>& other) noexcept
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
              typename = typename std::enable_if<constructible_from<U>::value>::type,
              typename std::enable_if<! convertible_from<U>::value, bool>::type = false>
    constexpr explicit optional_ref (const optional_ref<U>& other) noexcept
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
    GCH_NODISCARD 
    constexpr pointer get_pointer (void) const noexcept 
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
    GCH_NODISCARD 
    constexpr reference operator* (void) const noexcept 
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
    GCH_NODISCARD 
    constexpr pointer operator-> (void) const noexcept 
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
    GCH_NODISCARD
    constexpr bool has_value (void) const noexcept 
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
    GCH_NODISCARD 
    constexpr explicit operator bool (void) const noexcept 
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
    GCH_NODISCARD 
    GCH_CPP14_CONSTEXPR reference value (void) const
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
    template <typename U> GCH_NODISCARD 
    constexpr reference value_or (U& default_value) const noexcept
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
    GCH_NODISCARD
    constexpr const_reference value_or (const Value&& default_value) const noexcept
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
              typename = typename std::enable_if<constructible_from<U>::value>::type>
    GCH_NODISCARD
    constexpr const_reference value_or (const U&& default_value) const noexcept = delete;
  
    /**
     * Swap the contained reference with that of `other`.
     * 
     * @param other a reference to another `optional_ref`.
     */
    GCH_CONSTEXPR_SWAP void swap (optional_ref& other) noexcept 
    {
      using std::swap;
      swap (this->m_ptr, other.m_ptr);
    }
  
    /**
     * Reset the contained reference.
     * 
     * Internally, sets the pointer to `nullptr`.
     */
    GCH_CPP14_CONSTEXPR void reset (void) noexcept 
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
              typename = typename std::enable_if<constructible_from<U>::value>::type>
    GCH_CONSTEXPR_ADDRESSOF reference emplace (U& ref) noexcept
    {
      return *(m_ptr = static_cast<pointer> (std::addressof (ref)));
    }
  
    /**
     * A deleted version for convertible rvalue references.
     * 
     * We don't want to allow rvalue references because the internal pointer 
     * does not sustain the object lifetime.
     */
    template <typename U,
              typename = typename std::enable_if<constructible_from<U>::value>::type>
    reference emplace (const U&&) = delete;
    
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
              typename = typename std::enable_if<constructible_from<U>::value>::type> 
    GCH_CONSTEXPR_ADDRESSOF bool contains (U& ref) const noexcept
    {
      return static_cast<pointer> (std::addressof (ref)) == m_ptr;
    }
    
    /**
     * A deleted version for rvalue references.
     * 
     * Using `contains` with an rvalue reference is obviously
     * incorrectly written, so we might as well delete it.
     */
    template <typename U,
              typename = typename std::enable_if<constructible_from<U>::value>::type>
    bool contains (const U&&) const noexcept = delete;
    
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
   * We compare by value, not by address.
   * 
   * @tparam T the value type of `l`.
   * @tparam U the value type of `r`.
   * @param l an `optional_ref`.
   * @param r an `optional_ref`.
   * @return the result of the equality comparison.
   * 
   * @see std::optional::operator==
   */
  template <typename T, typename U> GCH_NODISCARD
  constexpr auto operator== (const optional_ref<T>& l, const optional_ref<U>& r)
    noexcept (noexcept (*l == *r)) -> decltype (*l == *r)
  {
    return (l.has_value () == r.has_value ()) && (! l.has_value () || (*l == *r));
  }
  
  /**
   * An inequality comparison function.
   * 
   * We compare by value, not by address.
   * 
   * @tparam T the value type of `l`.
   * @tparam U the value type of `r`.
   * @param l an `optional_ref`.
   * @param r an `optional_ref`.
   * @return the result of the inequality comparison.
   * 
   * @see std::optional::operator!=
   */
  template <typename T, typename U> GCH_NODISCARD
  constexpr auto operator!= (const optional_ref<T>& l, const optional_ref<U>& r)
    noexcept (noexcept (*l != *r)) -> decltype (*l != *r)
  {
    return (l.has_value () != r.has_value ()) || (l.has_value () && (*l != *r));
  }
  
  /**
   * An less-than comparison function.
   * 
   * We compare by value, not by address.
   * 
   * @tparam T the value type of `l`.
   * @tparam U the value type of `r`.
   * @param l an `optional_ref`.
   * @param r an `optional_ref`.
   * @return the result of the less-than comparison.
   * 
   * @see std::optional::operator<
   */
  template <typename T, typename U> GCH_NODISCARD
  constexpr auto operator< (const optional_ref<T>& l, const optional_ref<U>& r)
    noexcept (noexcept (*l < *r)) -> decltype (*l < *r)
  {
    return r.has_value () && (! l.has_value () || (*l < *r));
  }
  
  /**
   * A greater-than comparison function.
   * 
   * We compare by value, not by address.
   * 
   * @tparam T the value type of `l`.
   * @tparam U the value type of `r`.
   * @param l an `optional_ref`.
   * @param r an `optional_ref`.
   * @return the result of the greater-than comparison.
   * 
   * @see std::optional::operator>
   */
  template <typename T, typename U> GCH_NODISCARD
  constexpr auto operator> (const optional_ref<T>& l, const optional_ref<U>& r)
    noexcept (noexcept (*l > *r)) -> decltype (*l > *r)
  {
    return l.has_value () && (! r.has_value () || (*l > *r));
  }
  
  /**
   * A less-than-equal comparison function.
   * 
   * We compare by value, not by address.
   * 
   * @tparam T the value type of `l`.
   * @tparam U the value type of `r`.
   * @param l an `optional_ref`.
   * @param r an `optional_ref`.
   * @return the result of the less-than-equal comparison.
   * 
   * @see std::optional::operator<=
   */
  template <typename T, typename U> GCH_NODISCARD
  constexpr auto operator<= (const optional_ref<T>& l, const optional_ref<U>& r)
    noexcept (noexcept (*l <= *r)) -> decltype (*l <= *r)
  {
    return ! l.has_value () || (r.has_value () && (*l <= *r));
  }
  
  /**
   * A greater-than-equal comparison function.
   * 
   * We compare by value, not by address.
   * 
   * @tparam T the value type of `l`.
   * @tparam U the value type of `r`.
   * @param l an `optional_ref`.
   * @param r an `optional_ref`.
   * @return the result of the greater-than-equal comparison.
   * 
   * @see std::optional::operator>=
   */
  template <typename T, typename U> GCH_NODISCARD
  constexpr auto operator>= (const optional_ref<T>& l, const optional_ref<U>& r)
    noexcept (noexcept (*l >= *r)) -> decltype (*l >= *r)
  {
    return ! r.has_value () || (l.has_value () && (*l >= *r));
  }
  
  /**
   * An equality comparison function.
   * 
   * We compare by value, not by address.
   * 
   * @tparam T the value type of `l`.
   * @param l an `optional_ref`.
   * @param r a `nullopt_t`.
   * @return the result of the equality comparison.
   * 
   * @see std::optional::operator==
   */
  template <typename T> GCH_NODISCARD
  constexpr bool operator== (const optional_ref<T>& l, nullopt_t) noexcept 
  {
    return ! l.has_value ();
  }
  
  /**
   * An equality comparison function.
   * 
   * We compare by value, not by address.
   * 
   * @tparam T the value type of `r`.
   * @param l a `nullopt_t`.
   * @param r an `optional_ref`.
   * @return the result of the equality comparison.
   * 
   * @see std::optional::operator==
   */
  template <typename T> GCH_NODISCARD
  constexpr bool operator== (nullopt_t, const optional_ref<T>& r) noexcept
  {
    return ! r.has_value ();
  }
  
  /**
   * An inequality comparison function.
   * 
   * We compare by value, not by address.
   * 
   * @tparam T the value type of `l`.
   * @param l an `optional_ref`.
   * @param r a `nullopt_t`.
   * @return the result of the inequality comparison.
   * 
   * @see std::optional::operator!=
   */
  template <typename T> GCH_NODISCARD
  constexpr bool operator!= (const optional_ref<T>& l, nullopt_t) noexcept
  {
    return l.has_value ();
  }
  
  /**
   * An inequality comparison function.
   * 
   * We compare by value, not by address.
   * 
   * @tparam T the value type of `r`.
   * @param l a `nullopt_t`.
   * @param r an `optional_ref`.
   * @return the result of the inequality comparison.
   * 
   * @see std::optional::operator!=
   */
  template <typename T> GCH_NODISCARD
  constexpr bool operator!= (nullopt_t, const optional_ref<T>& r) noexcept
  {
    return r.has_value ();
  }
  
  /**
   * A less-than comparison function.
   * 
   * We compare by value, not by address.
   * 
   * @tparam T the value type of `l`.
   * @param l an `optional_ref`.
   * @param r a `nullopt_t`.
   * @return the result of the less-than comparison.
   * 
   * @see std::optional::operator<
   */
  template <typename T> GCH_NODISCARD
  constexpr bool operator< (const optional_ref<T>&, nullopt_t) noexcept
  {
    return false;
  }
  
  /**
   * A less-than comparison function.
   * 
   * We compare by value, not by address.
   * 
   * @tparam T the value type of `r`.
   * @param l a `nullopt_t`.
   * @param r an `optional_ref`.
   * @return the result of the less-than comparison.
   * 
   * @see std::optional::operator<
   */
  template <typename T> GCH_NODISCARD
  constexpr bool operator< (nullopt_t, const optional_ref<T>& r) noexcept
  {
    return r.has_value ();
  }
  
  /**
   * A greater-than comparison function.
   * 
   * We compare by value, not by address.
   * 
   * @tparam T the value type of `l`.
   * @param l an `optional_ref`.
   * @param r a `nullopt_t`.
   * @return the result of the greater-than comparison.
   * 
   * @see std::optional::operator>
   */
  template <typename T> GCH_NODISCARD
  constexpr bool operator> (const optional_ref<T>& l, nullopt_t) noexcept
  {
    return l.has_value ();
  }
  
  /**
   * A greater-than comparison function.
   * 
   * We compare by value, not by address.
   * 
   * @tparam T the value type of `r`.
   * @param l a `nullopt_t`.
   * @param r an `optional_ref`.
   * @return the result of the greater-than comparison.
   * 
   * @see std::optional::operator>
   */
  template <typename T> GCH_NODISCARD
  constexpr bool operator> (nullopt_t, const optional_ref<T>&) noexcept
  {
    return false;
  }
  
  /**
   * A less-than-equal comparison function.
   * 
   * We compare by value, not by address.
   * 
   * @tparam T the value type of `l`.
   * @param l an `optional_ref`.
   * @param r a `nullopt_t`.
   * @return the result of the less-than-equal comparison.
   * 
   * @see std::optional::operator<=
   */
  template <typename T> GCH_NODISCARD
  constexpr bool operator<= (const optional_ref<T>& l, nullopt_t) noexcept
  {
    return ! l.has_value ();
  }
  
  /**
   * A less-than-equal comparison function.
   * 
   * We compare by value, not by address.
   * 
   * @tparam T the value type of `r`.
   * @param l a `nullopt_t`.
   * @param r an `optional_ref`.
   * @return the result of the less-than-equal comparison.
   * 
   * @see std::optional::operator<=
   */
  template <typename T> GCH_NODISCARD
  constexpr bool operator<= (nullopt_t, const optional_ref<T>&) noexcept
  {
    return true;
  }
  
  /**
   * A greater-than-equal comparison function.
   * 
   * We compare by value, not by address.
   * 
   * @tparam T the value type of `l`.
   * @param l an `optional_ref`.
   * @param r a `nullopt_t`.
   * @return the result of the greater-than-equal comparison.
   * 
   * @see std::optional::operator>=
   */
  template <typename T> GCH_NODISCARD
  constexpr bool operator>= (const optional_ref<T>&, nullopt_t) noexcept
  {
    return true;
  }
  
  /**
   * A greater-than-equal comparison function.
   * 
   * We compare by value, not by address.
   * 
   * @tparam T the value type of `r`.
   * @param l a `nullopt_t`.
   * @param r an `optional_ref`.
   * @return the result of the greater-than-equal comparison.
   * 
   * @see std::optional::operator>=
   */
  template <typename T> GCH_NODISCARD
  constexpr bool operator>= (nullopt_t, const optional_ref<T>& r) noexcept
  {
    return ! r.has_value ();
  }
  
  /**
   * An equality comparison function.
   * 
   * We compare by value, not by address.
   * 
   * @tparam T the value type of `l`.
   * @tparam U a value type which is comparable to T&.
   * @param l an `optional_ref`.
   * @param r a comparable lvalue reference.
   * @return the result of the equality comparison.
   * 
   * @see std::optional::operator==
   */
  template <typename T, typename U> GCH_NODISCARD
  constexpr auto operator== (const optional_ref<T>& l, const U& r)
    noexcept (noexcept (*l == r)) -> decltype (*l == r)
  {
    return (l.has_value () && (*l == r));
  }
  
  /**
   * An equality comparison function.
   * 
   * We compare by value, not by address.
   * 
   * @tparam T the value type of `r`.
   * @tparam U a value type which is comparable to T&.
   * @param l a comparable lvalue reference.
   * @param r an `optional_ref`.
   * @return the result of the equality comparison.
   * 
   * @see std::optional::operator==
   */
  template <typename T, typename U> GCH_NODISCARD
  constexpr auto operator== (const U& l, const optional_ref<T>& r)
    noexcept (noexcept (l == *r)) -> decltype (l == *r)
  {
    return (r.has_value () && (l == *r));
  }
  
  /**
   * An inequality comparison function.
   * 
   * We compare by value, not by address.
   * 
   * @tparam T the value type of `l`.
   * @tparam U a value type which is comparable to T&.
   * @param l an `optional_ref`.
   * @param r a comparable lvalue reference.
   * @return the result of the inequality comparison.
   * 
   * @see std::optional::operator!=
   */
  template <typename T, typename U> GCH_NODISCARD
  constexpr auto operator!= (const optional_ref<T>& l, const U& r)
    noexcept (noexcept (*l != r)) -> decltype (*l != r)
  {
    return (! l.has_value () || (*l != r));
  }
  
  /**
   * An inequality comparison function.
   * 
   * We compare by value, not by address.
   * 
   * @tparam T the value type of `r`.
   * @tparam U a value type which is comparable to T&.
   * @param l a comparable lvalue reference.
   * @param r an `optional_ref`.
   * @return the result of the inequality comparison.
   * 
   * @see std::optional::operator!=
   */
  template <typename T, typename U> GCH_NODISCARD
  constexpr auto operator!= (const U& l, const optional_ref<T>& r)
    noexcept (noexcept (l != *r)) -> decltype (l != *r)
  {
    return (! r.has_value () || (l != *r));
  }
  
  /**
   * A less-than comparison function.
   * 
   * We compare by value, not by address.
   * 
   * @tparam T the value type of `l`.
   * @tparam U a value type which is comparable to T&.
   * @param l an `optional_ref`.
   * @param r a comparable lvalue reference.
   * @return the result of the less-than comparison.
   * 
   * @see std::optional::operator<
   */
  template <typename T, typename U> GCH_NODISCARD
  constexpr auto operator< (const optional_ref<T>& l, const U& r)
    noexcept (noexcept (*l < r)) -> decltype (*l < r)
  {
    return (! l.has_value () || (*l < r));
  }
  
  /**
   * A less-than comparison function.
   * 
   * We compare by value, not by address.
   * 
   * @tparam T the value type of `r`.
   * @tparam U a value type which is comparable to T&.
   * @param l a comparable lvalue reference.
   * @param r an `optional_ref`.
   * @return the result of the less-than comparison.
   * 
   * @see std::optional::operator<
   */
  template <typename T, typename U> GCH_NODISCARD
  constexpr auto operator< (const U& l, const optional_ref<T>& r)
    noexcept (noexcept (l < *r)) -> decltype (l < *r)
  {
    return (r.has_value () && (l < *r));
  }
  
  /**
   * A greater-than comparison function.
   * 
   * We compare by value, not by address.
   * 
   * @tparam T the value type of `l`.
   * @tparam U a value type which is comparable to T&.
   * @param l an `optional_ref`.
   * @param r a comparable lvalue reference.
   * @return the result of the greater-than comparison.
   * 
   * @see std::optional::operator>
   */
  template <typename T, typename U> GCH_NODISCARD
  constexpr auto operator> (const optional_ref<T>& l, const U& r)
    noexcept (noexcept (*l > r)) -> decltype (*l > r)
  {
    return (l.has_value () && (*l > r));
  }
  
  /**
   * A greater-than comparison function.
   * 
   * We compare by value, not by address.
   * 
   * @tparam T the value type of `r`.
   * @tparam U a value type which is comparable to T&.
   * @param l a comparable lvalue reference.
   * @param r an `optional_ref`.
   * @return the result of the greater-than comparison.
   * 
   * @see std::optional::operator>
   */
  template <typename T, typename U> GCH_NODISCARD
  constexpr auto operator> (const U& l, const optional_ref<T>& r)
    noexcept (noexcept (l > *r)) -> decltype (l > *r)
  {
    return (! r.has_value () || (l > *r));
  }
  
  /**
   * A less-than-equal comparison function.
   * 
   * We compare by value, not by address.
   * 
   * @tparam T the value type of `l`.
   * @tparam U a value type which is comparable to T&.
   * @param l an `optional_ref`.
   * @param r a comparable lvalue reference.
   * @return the result of the less-than-equal comparison.
   * 
   * @see std::optional::operator<=
   */
  template <typename T, typename U> GCH_NODISCARD
  constexpr auto operator<= (const optional_ref<T>& l, const U& r)
    noexcept (noexcept (*l <= r)) -> decltype (*l <= r)
  {
    return (! l.has_value () || (*l <= r));
  }
  
  /**
   * A less-than-equal comparison function.
   * 
   * We compare by value, not by address.
   * 
   * @tparam T the value type of `r`.
   * @tparam U a value type which is comparable to T&.
   * @param l a comparable lvalue reference.
   * @param r an `optional_ref`.
   * @return the result of the less-than-equal comparison.
   * 
   * @see std::optional::operator<=
   */
  template <typename T, typename U> GCH_NODISCARD
  constexpr auto operator<= (const U& l, const optional_ref<T>& r)
    noexcept (noexcept (l <= *r)) -> decltype (l <= *r)
  {
    return (r.has_value () && (l <= *r));
  }
  
  /**
   * A greater-than-equal comparison function.
   * 
   * We compare by value, not by address.
   * 
   * @tparam T the value type of `l`.
   * @tparam U a value type which is comparable to T&.
   * @param l an `optional_ref`.
   * @param r a comparable lvalue reference.
   * @return the result of the greater-than-equal comparison.
   * 
   * @see std::optional::operator>=
   */
  template <typename T, typename U> GCH_NODISCARD
  constexpr auto operator>= (const optional_ref<T>& l, const U& r)
    noexcept (noexcept (*l >= r)) -> decltype (*l >= r)
  {
    return (l.has_value () && (*l >= r));
  }
  
  /**
   * A greater-than-equal comparison function.
   * 
   * We compare by value, not by address.
   * 
   * @tparam T the value type of `r`.
   * @tparam U a value type which is comparable to T&.
   * @param l a comparable lvalue reference.
   * @param r an `optional_ref`.
   * @return the result of the greater-than-equal comparison.
   * 
   * @see std::optional::operator>=
   */
  template <typename T, typename U> GCH_NODISCARD
  constexpr auto operator>= (const U& l, const optional_ref<T>& r)
    noexcept (noexcept (l >= *r)) -> decltype (l >= *r)
  {
    return (! r.has_value () || (l >= *r));
  }
  
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
  GCH_CONSTEXPR_SWAP inline void 
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
  GCH_NODISCARD
  GCH_CONSTEXPR_ADDRESSOF optional_ref<typename std::remove_reference<U>::type> 
  make_optional_ref (U&& ref) noexcept
  {
    return optional_ref<typename std::remove_reference<U>::type> { std::forward<U> (ref) };
  }

#ifdef GCH_DEDUCTION_GUIDE_SUPPORT
  template <typename U>
  optional_ref (U&&) -> optional_ref<std::remove_reference_t<U>>;
#endif
  
}

/**
 * A specialization of `std::hash` for `gch::optional_ref`.
 * 
 * @tparam T the value type of `gch::optional_ref`.
 */
template <typename T>
struct std::hash<gch::optional_ref<T>>
{
  /**
   * An invokable operator.
   * 
   * We just do a noop pointer hash (which is unique).
   * 
   * @param opt_ref a reference to a value of type `gch::optional_ref`.
   * @return a hash of the argument.
   */
  constexpr std::size_t operator() (const gch::optional_ref<T>& opt_ref) const noexcept
  {
    return reinterpret_cast<std::size_t> (opt_ref.get_pointer ());
  }
};

#undef GCH_NODISCARD
#undef GCH_CONSTEXPR_ADDRESSOF
#undef GCH_INLINE_VARS
#undef GCH_CONSTEXPR_SWAP
#undef GCH_CPP14_CONSTEXPR
#undef GCH_DEDUCTION_GUIDE_SUPPORT

#endif
