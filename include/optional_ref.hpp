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

#include <functional>

namespace gch
{
  struct nullopt_t
  {
    enum class engage { value };
    explicit constexpr nullopt_t (engage) noexcept { }
  };
  
  static constexpr nullopt_t nullopt { nullopt_t::engage::value };
  
  class bad_optional_access : public std::exception
  {
  public:
    bad_optional_access (void)  = default;
    virtual ~bad_optional_access ( ) noexcept = default;
    const char* what (void) const noexcept override { return "bad optional_ref access"; }
  };
  
  template <typename Value>
  class optional_ref
  {   
  public:
    using value_type = Value;
    using reference  = typename std::add_lvalue_reference<Value>::type;
    using pointer    = typename std::add_pointer<Value>::type;
    
  private:
    
    template <typename U>
    using constructible_from = std::is_constructible<pointer, typename optional_ref<U>::pointer>;
  
    template <typename U>
    using convertible_from = std::is_convertible<typename optional_ref<U>::pointer, pointer>;
    
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
    
    constexpr /* implicit */ optional_ref (nullopt_t) noexcept { };
    
    template <typename U, 
              typename = typename std::enable_if<! is_optional_ref<U>::value>::type,
              typename = typename std::enable_if<std::is_rvalue_reference<U>::value>::type>
    optional_ref (U&&) = delete;
  
    template <typename U,
              typename = typename std::enable_if<constructible_from<U>::value>::type,
              typename std::enable_if<convertible_from<U>::value, bool>::type = true
             >
    constexpr /* implicit */ optional_ref (U& ref) noexcept
      : m_ptr (std::addressof (ref))
    { }
  
    template <typename U,
              typename = typename std::enable_if<constructible_from<U>::value>::type,
              typename std::enable_if<! convertible_from<U>::value, bool>::type = false
             >
    constexpr explicit optional_ref (U& ref) noexcept
      : m_ptr (std::addressof (static_cast<reference> (ref)))
    { }
  
    template <typename U,
              typename = typename std::enable_if<constructible_from<U>::value>::type,
              typename std::enable_if<convertible_from<U>::value, bool>::type = true
             >
    constexpr /* implicit */ optional_ref (const optional_ref<U>& other) noexcept
      : m_ptr (other.operator->())
    { }
  
    template <typename U,
              typename = typename std::enable_if<constructible_from<U>::value>::type,
              typename std::enable_if<! convertible_from<U>::value, bool>::type = false
             >
    constexpr explicit optional_ref (const optional_ref<U>& other) noexcept
        : m_ptr (static_cast<pointer> (other.operator->()))
    { }
    
    constexpr reference operator*     (void) const noexcept { return *m_ptr;           }
    constexpr pointer   operator->    (void) const noexcept { return m_ptr;            }
    constexpr bool      has_value     (void) const noexcept { return m_ptr != nullptr; }
    constexpr explicit  operator bool (void) const noexcept { return has_value ();     }

#if __cpp_constexpr > 200704L
    constexpr
#endif
    reference value (void) const
    {
      if (! has_value ())
        throw bad_optional_access();
      return *m_ptr;
    }
    
    template <typename U>
    constexpr reference value_or (U&& default_value) const noexcept
    {
      return has_value () ? *m_ptr : static_cast<reference> (std::forward<U> (default_value));
    }
    
    void swap (optional_ref& other) noexcept 
    {
      using std::swap;
      swap (this->m_ptr, other.m_ptr);
    }
    
    void reset (void) noexcept 
    {
      m_ptr = nullptr;
    }
    
    template <typename U,
              typename std::enable_if<std::is_lvalue_reference<U>::value, bool>::type = true>
    reference emplace (U&& ref)
    {
      return *(m_ptr = std::addressof (ref));
    }
  
    template <typename U,
              typename std::enable_if<! std::is_lvalue_reference<U>::value, bool>::type = false>
    reference emplace (U&&) = delete;

  private:
    
    pointer m_ptr { nullptr };
    
  };
  
  template <typename T, typename U>
  constexpr auto operator== (const optional_ref<T>& l, const optional_ref<U>& r) 
    noexcept (noexcept (*l == *r)) -> decltype (*l == *r)
  {
    return (l.has_value () == r.has_value ()) && (! l.has_value () || (*l == *r));
  }
  
  template <typename T, typename U>
  constexpr auto operator!= (const optional_ref<T>& l, const optional_ref<U>& r)
    noexcept (noexcept (*l != *r)) -> decltype (*l != *r)
  {
    return (l.has_value () != r.has_value ()) || (l.has_value () && (*l != *r));
  }
  
  template <typename T, typename U>
  constexpr auto operator< (const optional_ref<T>& l, const optional_ref<U>& r)
    noexcept (noexcept (*l < *r)) -> decltype (*l < *r)
  {
    return r.has_value () && (! l.has_value () || (*l < *r));
  }
  
  template <typename T, typename U>
  constexpr auto operator> (const optional_ref<T>& l, const optional_ref<U>& r)
    noexcept (noexcept (*l > *r)) -> decltype (*l > *r)
  {
    return l.has_value () && (! r.has_value () || (*l > *r));
  }
  
  template <typename T, typename U>
  constexpr auto operator<= (const optional_ref<T>& l, const optional_ref<U>& r)
    noexcept (noexcept (*l <= *r)) -> decltype (*l <= *r)
  {
    return ! l.has_value () || (r.has_value () && (*l <= *r));
  }
  
  template <typename T, typename U>
  constexpr auto operator>= (const optional_ref<T>& l, const optional_ref<U>& r)
    noexcept (noexcept (*l >= *r)) -> decltype (*l >= *r)
  {
    return ! r.has_value () || (l.has_value () && (*l >= *r));
  }
  
  template <typename T>
  constexpr bool operator== (const optional_ref<T>& l, nullopt_t) noexcept 
  {
    return ! l.has_value ();
  }
  
  template <typename T>
  constexpr bool operator== (nullopt_t, const optional_ref<T>& r) noexcept
  {
    return ! r.has_value ();
  }
  
  template <typename T>
  constexpr bool operator!= (const optional_ref<T>& l, nullopt_t) noexcept
  {
    return l.has_value ();
  }
  
  template <typename T>
  constexpr bool operator!= (nullopt_t, const optional_ref<T>& r) noexcept
  {
    return r.has_value ();
  }
  
  template <typename T>
  constexpr bool operator< (const optional_ref<T>&, nullopt_t) noexcept
  {
    return false;
  }
  
  template <typename T>
  constexpr bool operator< (nullopt_t, const optional_ref<T>& r) noexcept
  {
    return r.has_value ();
  }
  
  template <typename T>
  constexpr bool operator> (const optional_ref<T>& l, nullopt_t) noexcept
  {
    return l.has_value ();
  }
  
  template <typename T>
  constexpr bool operator> (nullopt_t, const optional_ref<T>&) noexcept
  {
    return false;
  }
  
  template <typename T>
  constexpr bool operator<= (const optional_ref<T>& l, nullopt_t) noexcept
  {
    return ! l.has_value ();
  }
  
  template <typename T>
  constexpr bool operator<= (nullopt_t, const optional_ref<T>&) noexcept
  {
    return true;
  }
  
  template <typename T>
  constexpr bool operator>= (const optional_ref<T>&, nullopt_t) noexcept
  {
    return true;
  }
  
  template <typename T>
  constexpr bool operator>= (nullopt_t, const optional_ref<T>& r) noexcept
  {
    return ! r.has_value ();
  }
  
  template <typename T, typename U>
  constexpr auto operator== (const optional_ref<T>& l, const U& r)
    noexcept (noexcept (*l == r)) -> decltype (*l == r)
  {
    return (l.has_value () && (*l == r));
  }
  
  template <typename T, typename U>
  constexpr auto operator== (const U& l, const optional_ref<T>& r)
    noexcept (noexcept (l == *r)) -> decltype (l == *r)
  {
    return (r.has_value () && (l == *r));
  }
  
  template <typename T, typename U>
  constexpr auto operator!= (const optional_ref<T>& l, const U& r)
    noexcept (noexcept (*l != r)) -> decltype (*l != r)
  {
    return (! l.has_value () || (*l != r));
  }
  
  template <typename T, typename U>
  constexpr auto operator!= (const U& l, const optional_ref<T>& r)
    noexcept (noexcept (l != *r)) -> decltype (l != *r)
  {
    return (! r.has_value () || (l != *r));
  }
  
  template <typename T, typename U>
  constexpr auto operator< (const optional_ref<T>& l, const U& r)
    noexcept (noexcept (*l < r)) -> decltype (*l < r)
  {
    return (! l.has_value () || (*l < r));
  }
  
  template <typename T, typename U>
  constexpr auto operator< (const U& l, const optional_ref<T>& r)
    noexcept (noexcept (l < *r)) -> decltype (l < *r)
  {
    return (r.has_value () && (l < *r));
  }
  
  template <typename T, typename U>
  constexpr auto operator> (const optional_ref<T>& l, const U& r)
    noexcept (noexcept (*l > r)) -> decltype (*l > r)
  {
    return (l.has_value () && (*l > r));
  }
  
  template <typename T, typename U>
  constexpr auto operator> (const U& l, const optional_ref<T>& r)
    noexcept (noexcept (l > *r)) -> decltype (l > *r)
  {
    return (! r.has_value () || (l > *r));
  }
  
  template <typename T, typename U>
  constexpr auto operator<= (const optional_ref<T>& l, const U& r)
    noexcept (noexcept (*l <= r)) -> decltype (*l <= r)
  {
    return (! l.has_value () || (*l <= r));
  }
  
  template <typename T, typename U>
  constexpr auto operator<= (const U& l, const optional_ref<T>& r)
    noexcept (noexcept (l <= *r)) -> decltype (l <= *r)
  {
    return (r.has_value () && (l <= *r));
  }
  
  template <typename T, typename U>
  constexpr auto operator>= (const optional_ref<T>& l, const U& r)
    noexcept (noexcept (*l >= r)) -> decltype (*l >= r)
  {
    return (l.has_value () && (*l >= r));
  }
  
  template <typename T, typename U>
  constexpr auto operator>= (const U& l, const optional_ref<T>& r)
    noexcept (noexcept (l >= *r)) -> decltype (l >= *r)
  {
    return (! r.has_value () || (l >= *r));
  }
  
  template <typename T>
  inline void swap (optional_ref<T>& l, optional_ref<T>& r) noexcept
  {
    l.swap (r);
  }
  
  template<typename T>
  constexpr optional_ref<typename std::remove_reference<T>::type>
  make_optional_ref (T&& ref)
  {
    return optional_ref<typename std::remove_reference<T>::type> { std::forward<T>(ref) }; 
  }
}

namespace std
{
  template <typename T>
  struct hash<gch::optional_ref<T>>
  {
  private:
    using pointer = typename gch::optional_ref<T>::pointer;
  public:
    std::size_t operator() (const gch::optional_ref<T>& opt_ref) const 
      noexcept (noexcept (std::hash<pointer> {} (opt_ref.operator-> ())))
    {
      return std::hash<pointer> {} (opt_ref.operator-> ());
    }
  };
}

#endif
