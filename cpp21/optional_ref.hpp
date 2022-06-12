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
  template<class T = void_t> class optional_ref_;
  template<class T = void_t> using optional_ref = optional_ref_<decay_t<T>>;

  // 
  template<class T>
  class optional_ref_ {
  protected:
    //wrap_ptr<T> ptr = nullptr;
    T* ptr = nullptr;

  public:

    // 
    inline optional_ref_(std::optional<T> const& ref) : ptr(const_cast<T*>(ref ? &(ref.value()) : nullptr)) {};
    inline optional_ref_(optional_ref<T> const& ref) : ptr(const_cast<T*>(ref ? &(ref.value()) : nullptr)) {};
    inline optional_ref_(T* ref) : ptr(ref) {};
    inline optional_ref_(T& ref) : ptr(&ref) {};
    inline optional_ref_(T&& ref) : ptr(&unmove(ref)) {};
    inline optional_ref_(T const& ref) : ptr(const_cast<T*>(&ref)) {};
    inline optional_ref_(wrap_ptr<T> ref) : ptr(ref.get()) {};
    inline optional_ref_() {};

    // check operator
    inline operator bool() const { return !!this->ptr; };

    // type conversion
    inline operator T& () { return *this->ptr; };
    inline operator T const& () const { return *this->ptr; };

    // type conversion
    inline operator T* () { return this->ptr; };
    inline operator T const* () const { return this->ptr; };

    // type conversion
    //inline operator wrap_ptr<T>& () { return this->ptr; };
    //inline operator wrap_ptr<T> const& () const { return this->ptr; };

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

    // assign const ref
    inline decltype(auto) operator=(std::optional<T> const& ref) { if (!ptr) { if (ref) { ptr = const_cast<T*>(&ref.value()); }; } else { ptr = ref ? reinterpret_cast<T*>(&ref.value()) : nullptr; }; return *this; };
    inline decltype(auto) operator=(optional_ref<T> const& ref) { if (!ptr) { if (ref) { ptr = const_cast<T*>(&ref.value()); }; } else { ptr = ref ? reinterpret_cast<T*>(&ref.value()) : nullptr; }; return *this; };

    // assign ref
    inline decltype(auto) operator=(std::optional<T>& ref) { if (!ptr) { if (ref) { ptr = &ref.value(); }; } else { ptr = ref ? reinterpret_cast<T*>(&ref.value()) : nullptr; }; return *this; };
    inline decltype(auto) operator=(optional_ref<T>& ref) { if (!ptr) { if (ref) { ptr = &ref.value(); }; } else { ptr = ref ? reinterpret_cast<T*>(&ref.value()) : nullptr; }; return *this; };

    //
    inline decltype(auto) operator=(wrap_ptr<T> const& ptx) { if (!ptr) { ptr = const_cast<T*>(ptx); } else { ptr = ptx; }; return *this; };
    inline decltype(auto) operator=(T const& ref) { if (!ptr) { ptr = const_cast<T*>(&ref); } else { *ptr = ref; }; return *this; };
    inline decltype(auto) operator=(T const* ptx) { if (!ptr) { ptr = const_cast<T*>(ptx); } else { ptr = ptx; }; return *this; };

    // value alias
    inline decltype(auto) value() { return *this->ptr; };
    inline decltype(auto) value() const { return *this->ptr; };

    // value alias
    [[deprecated]] inline decltype(auto) ref() { return *this->ptr; };
    [[deprecated]] inline decltype(auto) ref() const { return *this->ptr; };

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
