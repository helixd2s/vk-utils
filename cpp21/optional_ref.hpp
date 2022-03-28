#pragma once 

// 
#ifdef __cplusplus
#include "./core.hpp"
#include "./wrap_ptr.hpp"

// 
namespace cpp21 {

  /*
      // What is optional ref pointer? This is our implementation optional_ref from type_safe, more functional.
  */

  // 
  template<class T = void_t>
  class optional_ref {
  protected:
    wrap_ptr<T> ptr = nullptr;

  public:
    // 
#ifdef TYPE_SAFE_OPTIONAL_REF_HPP_INCLUDED
    inline optional_ref(ts::optional_ref<T> const& ref) : ptr(ref ? ref.value() : nullptr) {};
    inline optional_ref(ts::optional_ref<T>& ref) : ptr(ref ? ref.value() : nullptr) {};
#endif

    // 
    inline optional_ref(std::optional<T> const& ref) : ptr(ref ? &(ref.value()) : nullptr) {};
    inline optional_ref(std::optional<T>& ref) : ptr(ref ? &(ref.value()) : nullptr) {};

    // 
    inline optional_ref(optional_ref<T> const& ref) : ptr(ref ? &(ref.value()) : nullptr) {};
    inline optional_ref(optional_ref<T>& ref) : ptr(ref ? &(ref.value()) : nullptr) {};

    // 
    inline optional_ref(T* ref) : ptr(ref) {};
    inline optional_ref(T& ref) : ptr(&ref) {};
    inline optional_ref() {};

    // check operator
    inline operator bool() const { return !!this->ptr; };

    // type conversion
    inline operator T& () { return *this->ptr; };
    inline operator T const& () const { return *this->ptr; };

    // type conversion
    inline operator T* () { return this->ptr; };
    inline operator T const* () const { return this->ptr; };

    // type conversion
    inline operator wrap_ptr<T>& () { return this->ptr; };
    inline operator wrap_ptr<T> const& () const { return this->ptr; };

    //
#ifdef TYPE_SAFE_OPTIONAL_REF_HPP_INCLUDED
    inline ts::optional_ref<T>() { return ts::opt_ref(*this->ptr); };
    inline ts::optional_ref<const T>() const { return ts::opt_cref(*this->ptr); };
#endif

    //
#ifdef TYPE_SAFE_OPTIONAL_REF_HPP_INCLUDED
    inline decltype(auto) operator=(ts::optional_ref<T> const& ref) { ptr = ref ? &ref.value() : nullptr; return *this; };
    inline decltype(auto) operator=(ts::optional_ref<T>& ref) { ptr = ref ? &ref.value() : nullptr; return *this; };
#endif

    // assign ref
    inline decltype(auto) operator=(std::optional<T>& ref) { ptr = ref ? &ref.value() : nullptr; return *this; };
    inline decltype(auto) operator=(optional_ref<T> const& ref) { ptr = ref ? &ref.value() : nullptr; return *this; };
    inline decltype(auto) operator=(optional_ref<T>& ref) { ptr = ref ? &ref.value() : nullptr; return *this; };
    inline decltype(auto) operator=(wrap_ptr<T> const& ptx) { ptr = ptx; return *this; };
    inline decltype(auto) operator=(T const& ref) { *ptr = ref; return *this; };
    inline decltype(auto) operator=(T const* ptx) { ptr = ptx; return *this; };

    // value alias
    inline decltype(auto) value() { return *this->ptr; };
    inline decltype(auto) value() const { return *this->ptr; };

    // accessing operator
    inline decltype(auto) operator *() { return *this->ptr; };
    inline decltype(auto) operator *() const { return *this->ptr; };

    // const accessing operator
    inline decltype(auto) operator ->() { return this->ptr; };
    inline decltype(auto) operator ->() const { return this->ptr; };

    // because it's reference, pointer must got directly...
    inline decltype(auto) operator&() { return this->ptr; };
    inline decltype(auto) operator&() const { return this->ptr; };

    // proxy...
    inline decltype(auto) operator[](uintptr_t const& index) { return (*this->ptr)[index]; };
    inline decltype(auto) operator[](uintptr_t const& index) const { return (*this->ptr)[index]; };
  };

  // 
  template<class T = void_t>
  inline decltype(auto) opt_ref(T& ref) {
    //using T = std::decay_t<decltype(ref)>;
    return optional_ref<T>(ref);
  };

  // 
  template<class T = void_t>
  inline decltype(auto) opt_cref(const T& ref) {
    using T = const std::decay_t<decltype(ref)>;
    return optional_ref<T>(ref);
  };

  //
  template<class Ts = void_t>
  inline decltype(auto) pointer(optional_ref<Ts> ref) {
    Ts* pt = ref ? (&ref.value()) : nullptr;
    return wrap_ptr<Ts>(pt);
  };

  //
  template<class Ts = void_t>
  inline decltype(auto) pointer(optional_ref<const Ts> ref) {
    Ts* pt = ref ? (&ref.value()) : nullptr;
    return wrap_ptr<Ts>(pt);
  };

  //
  template<class Ts = void_t>
  inline decltype(auto) pointer(std::optional<Ts>& ref) {
    Ts* pt = ref ? (&ref.value()) : nullptr;
    return wrap_ptr<Ts>(pt);
  };

};
#endif
