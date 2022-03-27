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
  template<class T, class wT>
  class wrap_ptr {
  protected:
    wT* ptr = nullptr;

  public:
    // 
    inline wrap_ptr(std::optional<wT> const& opt) : ptr(opt ? &opt.value() : void_t{}) {};
    inline wrap_ptr(wrap_ptr<wT> const& wrap) : ptr(const_cast<T*>(wrap.get())) {};
    inline wrap_ptr(wT* ref = nullptr) : ptr(ref) {};
    inline wrap_ptr(wT& ref) : ptr(&ref) {};

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
    inline decltype(auto) operator&() { return &ptr; };
    inline decltype(auto) operator&() const { return &ptr; };

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
    inline decltype(auto) operator =(wrap_ptr<wT> && wrap) { this->ptr = wrap.get(); return *this; };
    inline decltype(auto) operator =(wrap_ptr<wT> const& wrap) { this->ptr = const_cast<wT*>(wrap.get()); return *this; };
    inline decltype(auto) operator =(wrap_ptr<wT> & wrap) { this->ptr = wrap.get(); return *this; };
    inline decltype(auto) operator =(auto* const& ref) { using Tm = std::remove_cv_t<std::decay_t<decltype(ref)>>; this->ptr = (wT*)(ref); return *this; };
    inline decltype(auto) operator =(auto const& ref) { using Tm = std::remove_cv_t<std::decay_t<decltype(ref)>>; this->ptr = (wT*)(&ref); return *this; };
    inline decltype(auto) operator =(auto const* ref) { using Tm = std::remove_cv_t<std::decay_t<decltype(ref)>>; this->ptr = (wT*)(ref); return *this; };
    inline decltype(auto) operator =(auto& ref) { using Tm = std::remove_cv_t<std::decay_t<decltype(ref)>>; this->ptr = (wT*)(&ref); return *this; };
    //inline decltype(auto) operator =(auto* ref) { using Tm = std::remove_cv_t<std::decay_t<decltype(ref)>>; this->ptr = (wT*)(ref); return *this; };

    // 
    inline operator wT& () { return *this->get(); };
    inline operator wT const& () const { return *this->get(); };

    // 
    inline operator wT*& () { return this->get(); };
    inline operator wT const* const& () const { return this->get(); };

    // 
    inline operator bool() const { return !!ptr; };
  };

  //
  template<class Ts = void_t, class wT = std::decay_t<Ts>>
  inline decltype(auto) pointer(std::optional<wT> const& ref) {
    wT* pt = const_cast<wT*>(ref ? (&ref.value()) : nullptr);
    return wrap_ptr<wT>(pt);
  };

  //
  template<class Ts = void_t, class wT = std::decay_t<Ts>>
  inline decltype(auto) pointer(std::optional<const wT> const& ref) {
    wT* pt = const_cast<wT*>(ref ? (&ref.value()) : nullptr);
    return wrap_ptr<wT>(pt);
  };

  // 
  template<class T>
  class wrap_shared_ptr {
  public:
  protected:
    using wT = std::remove_cv_t<std::decay_t<std::remove_cv_t<T>>>;
    using St = std::shared_ptr<wT>;
    St ptr = {};

  public:
    // 
    inline wrap_shared_ptr(St shp = {}) : ptr(shp) {};

    //
    //inline ~wrap_shared_ptr() { delete this->ptr; }

    //
    inline operator bool() const { return !!this->ptr; };

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
    inline operator wT& () { return this->ref(); };
    inline operator wT const& () const { return this->ref(); };

    //
    inline operator wT* () { return this->get(); };
    inline operator wT const* () const { return this->get(); };

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


  template<class T, class wT = std::decay_t<T>>
  inline typename std::remove_reference<T>::type* rvalue_to_ptr(T&& t)
  {
    return &t;
  };

  template<class T> T& unmove(T&& t) { return t; }

  // 
  template<class T>
  class const_wrap_arg {
  protected:
    using wT = std::remove_cv_t<std::decay_t<std::remove_cv_t<T>>>;
    wT const* ptr = nullptr;
    std::optional<wT> temp = {};

  public:
    const_wrap_arg() {};

    const_wrap_arg(T&& rvalue) { temp = std::move(rvalue), const_cast<wT*&>(ptr) = &temp.value(); };
    const_wrap_arg(wT& lvalue) : ptr(&lvalue) {}; // ambigous?
    const_wrap_arg(wT const& lcvalue) : ptr(&lcvalue) {};
    const_wrap_arg(wT const* pcvalue) : ptr(pcvalue) {};
    const_wrap_arg(const_wrap_arg<wT> const& wvalue) : ptr(wvalue.get()) { temp = wvalue.temp; const_cast<wT*&>(ptr) = temp ? &temp.value() : const_cast<wT*&>(wvalue.ptr); };
    const_wrap_arg(wrap_ptr<wT> const& wvalue) : ptr(wvalue.get()) {};
    const_wrap_arg(std::optional<wT> const& lcvalue) : ptr(lcvalue ? &lcvalue.value() : nullptr) {};

    //
    //const_wrap_arg(T&& rvalue) { temp = std::move(rvalue), const_cast<wT*&>(ptr) = &temp.value(); };

    //const_cast<std::remove_cv_t<std::decay_t<decltype(*ptr)>>&>()
    inline operator bool() const { return !!ptr; };
    inline operator std::optional<wT>() const { return this->optional(); };
    inline decltype(auto) optional() const { return ptr ? std::optional<wT>(const_cast<wT&>(*ptr)) : std::nullopt; };
    inline operator wT const* () const { return ptr; };
    inline operator wT const& () const { return (*ptr); };
    //inline operator wT* () { return const_cast<wT*&>(ptr); };
    //inline operator wT& () { return (*const_cast<wT*&>(ptr)); };

    //
    inline decltype(auto) operator=(std::optional<wT> const& ref) { ptr = ref ? &ref.value() : nullptr; return *this; };
    //inline decltype(auto) operator=(std::decay_t<T> & ref) { ptr = &ref; return *this; };
    inline decltype(auto) operator=(wT&& ref) { temp = std::move(ref), const_cast<wT*&>(ptr) = &temp.value(); return *this; };
    inline decltype(auto) operator=(wT const& ref) { ptr = &ref; return *this; };
    inline decltype(auto) operator=(wT const* ref) { ptr = ref; return *this; };
    inline decltype(auto) operator=(const_wrap_arg<wT> const& wvalue) { temp = wvalue.temp; const_cast<wT*&>(ptr) = temp ? &temp.value() : const_cast<wT*&>(wvalue.ptr); return *this; };
    inline decltype(auto) operator=(wrap_ptr<wT> const& ref) { ptr = ref; return *this; };

    // value alias
    //inline decltype(auto) value() { return *this->ptr; };
    inline decltype(auto) value() const { return unmove(*this->ptr); };

    //inline decltype(auto) get() { return this->ptr; };
    inline decltype(auto) get() const { return this->ptr; };
    //inline decltype(auto) get() { return *const_cast<wT*&>(this->ptr); };

    //
    inline decltype(auto) ref() const { return unmove(*this->get()); };
    //inline decltype(auto) ref() { return unmove(this->get()); };

    // accessing operator
    //inline decltype(auto) operator *() { return this->ref(); };
    inline decltype(auto) operator *() const { return this->ref(); };

    // const accessing operator
    //inline decltype(auto) operator ->() { return this->get(); };
    inline decltype(auto) operator ->() const { return this->get(); };

    // because it's reference, pointer must got directly...
    //inline decltype(auto) operator&() { return this->ref(); };
    inline decltype(auto) operator&() const { return this->ref(); };

    // proxy...
    //inline decltype(auto) operator[](uintptr_t const& index) { return (*this->ptr)[index]; };
    inline decltype(auto) operator[](uintptr_t const& index) const { return (*this->ptr)[index]; };
    inline decltype(auto) operator[](uint32_t const& index) const { return (*this->ptr)[index]; };
  };


};
