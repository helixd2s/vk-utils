#pragma once 

// 
#include "./core.hpp"
#include "./void.hpp"

// 
namespace cpp21 {

  /*
      // What is wrap_ptr? This is wrapper for C pointers, for avoid some conflicts and problems.
      // There is no specifical features, only pointers compatibility...
      // Size of is 8...
      // WARNING! Doesn't support "nullptr"! Needs workaround...
  */

  // if you needed just pointer, just cast `reinterpret_cast<T*&>(some_wrap_ptr)`, because contain only one pointer
  //template<class T = void_t>
  template<class T>
  class wrap_ptr {
  protected:
    T* ptr = nullptr;

  public:
    // 
    inline wrap_ptr(std::optional<T>& opt) : ptr(opt ? &opt.value() : void_t{}) {};
    inline wrap_ptr(wrap_ptr<T> const& wrap) : ptr(const_cast<T*>(wrap.get())) {};
    inline wrap_ptr(T* ref = nullptr) : ptr(ref) {};
    inline wrap_ptr(T& ref) : ptr(&ref) {};

    //
    inline decltype(auto) operator[](uintptr_t const& index) { return ptr[index]; };
    inline decltype(auto) operator[](uintptr_t const& index) const { return ptr[index]; };

    // 
    inline decltype(auto) operator->() { return this->get(); };
    inline decltype(auto) operator->() const { return this->get(); };

    // 
    inline decltype(auto) operator *() { return *this->get(); };
    inline decltype(auto) operator *() const { return *this->get(); };

    // 
    inline decltype(auto) operator&() { return wrap_ptr(this); };
    inline decltype(auto) operator&() const { return wrap_ptr(this); };

    // 
    inline decltype(auto) ref() { return *this->get(); };
    inline decltype(auto) ref() const { return *this->get(); };

    // 
    inline decltype(auto) value() { return *this->get(); };
    inline decltype(auto) value() const { return *this->get(); };

    //
    inline decltype(auto) get() { return reinterpret_cast<T*&>(ptr); };
    inline decltype(auto) get() const { return reinterpret_cast<T const* const&>(ptr); };

    //
    inline decltype(auto) operator =(wrap_ptr<T> const& wrap) { this->ptr = wrap.get(); return *this; };
    inline decltype(auto) operator =(auto* const& ref) { using Tm = std::remove_cv_t<std::decay_t<decltype(ref)>>; this->ptr = (T*)(ref); return *this; };
    inline decltype(auto) operator =(auto const& ref) { using Tm = std::remove_cv_t<std::decay_t<decltype(ref)>>; this->ptr = (T*)(&ref); return *this; };

    // 
    inline operator T& () { return *this->get(); };
    inline operator T const& () const { return *this->get(); };

    // 
    inline operator T*& () { return this->get(); };
    inline operator T const* const& () const { return this->get(); };

    // 
    inline operator bool() const { return !!ptr; };
  };

  //
  template<class Ts = void_t, template<class T = Ts> class W = wrap_ptr>
  inline decltype(auto) pointer(std::optional<Ts> const& ref) {
    Ts* pt = const_cast<Ts*>(ref ? (&ref.value()) : nullptr);
    return W<Ts>(pt);
  };

  //
  template<class Ts = void_t, template<class T = Ts> class W = wrap_ptr>
  inline decltype(auto) pointer(std::optional<const Ts> const& ref) {
    Ts* pt = const_cast<Ts*>(ref ? (&ref.value()) : nullptr);
    return W<Ts>(pt);
  };

  // 
  //template<class T, template<class Ts = T> class Sp = std::shared_ptr, template<class Ts = T> class W = wrap_ptr>
  template<class T, template<class Ts> class Sp, template<class T> class W>
  class wrap_shared_ptr {
  public:
  protected:
    using St = Sp<T>;
    St ptr = {};

  public:
    // 
    inline wrap_shared_ptr(St shp = {}) : ptr(shp) {};

    //
    //inline ~wrap_shared_ptr() { delete this->ptr; }

    //
    inline decltype(auto) shared() { return this->ptr; };
    inline decltype(auto) shared() const { return this->ptr; };

    //
    inline decltype(auto) get() { return this->ptr.get(); };
    inline decltype(auto) get() const { return this->ptr.get(); };

    //
    inline decltype(auto) ref() { return *this->get(); };
    inline decltype(auto) ref() const { return *this->get(); };

    //
    inline decltype(auto) value() { return *this->get(); };
    inline decltype(auto) value() const { return *this->get(); };

    //
    inline decltype(auto) assign(St const& ref) { this->ptr = ref; return *this; };
    inline decltype(auto) operator=(St const& ref) { return this->assign(ref); };

    //
    inline operator St& () { return this->ptr; };
    inline operator St const& () const { return this->ptr; };

    //
    inline operator T& () { return this->ref(); };
    inline operator T const& () const { return this->ref(); };

    //
    inline operator T* () { return this->get(); };
    inline operator T const* () const { return this->get(); };

    //
    inline decltype(auto) operator*() { return this->ref(); };
    inline decltype(auto) operator*() const { return this->ref(); };

    //
    inline decltype(auto) operator->() { return this->ptr.get(); };
    inline decltype(auto) operator->() const { return this->ptr.get(); };

    //
    //inline decltype(auto) operator&() { return this->get(); };
    //inline decltype(auto) operator&() const { return this->get(); };
  };


};
