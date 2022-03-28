#pragma once 

// 
#ifdef __cplusplus
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
    inline wrap_ptr(std::optional<T> const& opt) : ptr(opt ? &opt.value() : void_t{}) {};
    inline wrap_ptr(wrap_ptr<T> const& wrap) : ptr(const_cast<T*>(wrap.get())) {};
    inline wrap_ptr(T* ref = nullptr) : ptr(ref) {};
    inline wrap_ptr(T& ref) : ptr(&ref) {};

    //
    inline decltype(auto) operator[](uintptr_t const& index) { return ptr[index]; };
    inline decltype(auto) operator[](uintptr_t const& index) const { return ptr[index]; };

    // 
    inline decltype(auto) operator->() { return this->ptr; };
    inline decltype(auto) operator->() const { return this->ptr; };

    // 
    inline decltype(auto) operator *() { return *this->ptr; };
    inline decltype(auto) operator *() const { return *this->ptr; };

    // 
    inline decltype(auto) operator&() { return &this->ptr; };
    inline decltype(auto) operator&() const { return &this->ptr; };

    // 
    inline decltype(auto) ref() { return *this->ptr; };
    inline decltype(auto) ref() const { return *this->ptr; };

    // 
    inline decltype(auto) value() { return *this->ptr; };
    inline decltype(auto) value() const { return *this->ptr; };

    //
    inline decltype(auto) get() { return this->ptr; };
    inline decltype(auto) get() const { return this->ptr; };

    //
    inline decltype(auto) operator =(wrap_ptr<T> && wrap) { this->ptr = wrap.get(); return *this; };
    inline decltype(auto) operator =(wrap_ptr<T> const& wrap) { this->ptr = const_cast<T*>(wrap.get()); return *this; };
    inline decltype(auto) operator =(wrap_ptr<T> & wrap) { this->ptr = wrap.get(); return *this; };
    inline decltype(auto) operator =(auto* const& ref) { using Tm = std::remove_cv_t<std::decay_t<decltype(ref)>>; this->ptr = (T*)(ref); return *this; };
    inline decltype(auto) operator =(auto const& ref) { using Tm = std::remove_cv_t<std::decay_t<decltype(ref)>>; this->ptr = (T*)(&ref); return *this; };
    inline decltype(auto) operator =(auto const* ref) { using Tm = std::remove_cv_t<std::decay_t<decltype(ref)>>; this->ptr = (T*)(ref); return *this; };
    inline decltype(auto) operator =(auto& ref) { using Tm = std::remove_cv_t<std::decay_t<decltype(ref)>>; this->ptr = (T*)(&ref); return *this; };
    //inline decltype(auto) operator =(auto* ref) { using Tm = std::remove_cv_t<std::decay_t<decltype(ref)>>; this->ptr = (T*)(ref); return *this; };

    // 
    inline operator T& () { return *this->ptr; };
    inline operator T const& () const { return *this->ptr; };

    // 
    inline operator T*& () { return this->ptr; };
    inline operator T const* const& () const { return this->ptr; };

    // 
    inline operator bool() const { return !!ptr; };
  };

  //
  template<class Ts = void_t, class T = std::decay_t<Ts>>
  inline decltype(auto) pointer(std::optional<T> const& ref) {
    T* pt = const_cast<T*>(ref ? (&ref.value()) : nullptr);
    return wrap_ptr<T>(pt);
  };

  //
  template<class Ts = void_t, class T = std::decay_t<Ts>>
  inline decltype(auto) pointer(std::optional<const T> const& ref) {
    T* pt = const_cast<T*>(ref ? (&ref.value()) : nullptr);
    return wrap_ptr<T>(pt);
  };

  // 
  template<class T>
  class wrap_shared_ptr {
  public:
  protected:
    using St = std::shared_ptr<T>;
    St ptr = {};

  public:
    // 
    inline wrap_shared_ptr(St shp = {}) : ptr(shp) {};

    //
    inline operator bool() const { return !!this->ptr; };

    //
    inline decltype(auto) shared() { return this->ptr; };
    inline decltype(auto) shared() const { return this->ptr; };

    //
    inline decltype(auto) get() { return this->ptr.get(); };
    inline decltype(auto) get() const { return this->ptr.get(); };

    //
    inline decltype(auto) ref() { return *this->ptr; };
    inline decltype(auto) ref() const { return *this->ptr; };

    //
    inline decltype(auto) value() { return *this->ptr; };
    inline decltype(auto) value() const { return *this->ptr; };

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

  //
  template<class T>
  inline typename std::remove_reference<T>::type* rvalue_to_ptr(T&& t)
  {
    return &t;
  };

  // 
  template<class T> T& unmove(T&& t) { return t; }

  // 
  template<class T>
  class const_wrap_arg {
  protected:
    //using T = std::remove_cv_t<std::decay_t<std::remove_cv_t<T>>>;
    T const* ptr = nullptr;
    std::optional<T> temp = {};

  public:
    const_wrap_arg() {};
    //const_wrap_arg(T&& rvalue) { temp = std::move(rvalue), const_cast<T*&>(ptr) = &temp.value(); };
    const_wrap_arg(T const& lvalue) : ptr(&lvalue) { temp = std::move(lvalue), const_cast<T*&>(ptr) = &temp.value(); };
    const_wrap_arg(T const* pcvalue) : ptr(pcvalue) {};
    const_wrap_arg(const_wrap_arg<T> const& wvalue) : ptr(wvalue.get()) { temp = wvalue.temp; const_cast<T*&>(ptr) = temp ? &temp.value() : const_cast<T*&>(wvalue.ptr); };
    const_wrap_arg(wrap_ptr<T> const& wvalue) : ptr(wvalue.get()) {};
    const_wrap_arg(std::optional<T> const& lcvalue) : ptr(lcvalue ? &lcvalue.value() : nullptr) {};

    //
    inline operator bool() const { return !!ptr; };
    inline operator std::optional<T>() const { return this->optional(); };
    inline decltype(auto) optional() const { return ptr ? std::optional<T>(const_cast<T&>(*ptr)) : std::nullopt; };

    //
    inline operator T const* () const { return this->get(); };
    inline operator T const& () const { return this->ref(); };

    //
    inline operator T* () { return this->get(); };
    inline operator T& () { return this->ref(); };

    //
    inline decltype(auto) operator=(std::optional<T> const& ref) { ptr = ref ? &ref.value() : nullptr; return *this; };
    inline decltype(auto) operator=(T&& ref) { temp = std::move(ref), const_cast<T*&>(ptr) = &temp.value(); return *this; };
    inline decltype(auto) operator=(T const& ref) { ptr = &ref; return *this; };
    inline decltype(auto) operator=(T const* ref) { ptr = ref; return *this; };
    inline decltype(auto) operator=(const_wrap_arg<T> const& wvalue) { temp = wvalue.temp; const_cast<T*&>(ptr) = temp ? &temp.value() : const_cast<T*&>(wvalue.ptr); return *this; };
    inline decltype(auto) operator=(wrap_ptr<T> const& ref) { ptr = ref; return *this; };

    // value alias
    inline decltype(auto) value() { return *this->get(); };
    inline decltype(auto) value() const { return *this->get(); };

    // 
    inline decltype(auto) get() const { return this->ptr; };
    inline decltype(auto) get() { return const_cast<T*&>(this->ptr); };

    //
    inline decltype(auto) ref() const { return *this->get(); };
    inline decltype(auto) ref() { return *this->get(); };

    // accessing operator
    inline decltype(auto) operator *() { return this->ref(); };
    inline decltype(auto) operator *() const { return this->ref(); };

    // const accessing operator
    inline decltype(auto) operator ->() { return this->get(); };
    inline decltype(auto) operator ->() const { return this->get(); };

    // because it's reference, pointer must got directly...
    inline decltype(auto) operator&() { return this->ref(); };
    inline decltype(auto) operator&() const { return this->ref(); };

    // proxy...
    inline decltype(auto) operator[](uintptr_t const& index) const { return (*this->ptr)[index]; };
    inline decltype(auto) operator[](uint32_t const& index) const { return (*this->ptr)[index]; };
  };


};

#endif
