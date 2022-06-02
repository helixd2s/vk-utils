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

  //
  template<class T = void_t> class wrap_ptr_;
  template<class T = void_t> using wrap_ptr = wrap_ptr_<decay_t<T>>;

  // if you needed just pointer, just cast `reinterpret_cast<T*&>(some_wrap_ptr)`, because contain only one pointer
  //template<class T = void_t>
  template<class T>
  class wrap_ptr_ {
  protected:
    T* ptr = nullptr;

  public:
    // 
    inline wrap_ptr_(std::optional<T> const& opt) : ptr(opt ? &opt.value() : void_t{}) {};
    inline wrap_ptr_(wrap_ptr<T> const& wrap) : ptr(const_cast<T*>(wrap.get())) {};
    inline wrap_ptr_(T* ref = nullptr) : ptr(ref) {};
    inline wrap_ptr_(T& ref) : ptr(&ref) {};

    //
    inline decltype(auto) operator[](uintptr_t const& index) { return ptr[index]; };
    inline decltype(auto) operator[](uintptr_t const& index) const { return ptr[index]; };

    // 
    inline decltype(auto) operator->() { return this->ptr; };
    inline decltype(auto) operator->() const { return this->ptr; };

    // 
    inline auto& operator *() { return *this->ptr; };
    inline auto& operator *() const { return *this->ptr; };

    // 
    inline decltype(auto) operator&() { return &this->ptr; };
    inline decltype(auto) operator&() const { return &this->ptr; };

    // 
    inline auto& ref() { return *this->ptr; };
    inline auto& ref() const { return *this->ptr; };

    // 
    inline auto& value() { return *this->ptr; };
    inline auto& value() const { return *this->ptr; };

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
  template<class T = void_t> class wrap_weak_ptr_;
  template<class T = void_t> using wrap_weak_ptr = wrap_weak_ptr_<decay_t<T>>;

  // 
  template<class T>
  class wrap_weak_ptr_ {
  public:
  protected:
    using St = std::weak_ptr<T>;
    St ptr = {};

  public:
    // 
    inline wrap_weak_ptr_(std::shared_ptr<T> const& shp = {}) : ptr(shp) {};
    inline wrap_weak_ptr_(std::weak_ptr<T> const& shp) : ptr(shp.lock()) {};
    inline wrap_weak_ptr_(wrap_weak_ptr<T> const& shp) : ptr(shp.lock()) {};

    //
    inline operator bool() const { return !(this->ptr.expired()); };

    //
    inline decltype(auto) shared() { return this->ptr.lock(); };
    inline decltype(auto) shared() const { return this->ptr.lock(); };

    //
    inline decltype(auto) lock() { return this->ptr.lock(); };
    inline decltype(auto) lock() const { return this->ptr.lock(); };

    //
    inline std::weak_ptr<T> weak() { return this->ptr; };
    inline std::weak_ptr<T> weak() const { return this->ptr; };

    //
    inline decltype(auto) get() { return this->ptr.lock().get(); };
    inline decltype(auto) get() const { return this->ptr.lock().get(); };

    //
    inline auto& ref() { return *this->ptr.lock(); };
    inline auto& ref() const { return *this->ptr.lock(); };

    //
    inline auto& value() { return *this->ptr.lock(); };
    inline auto& value() const { return *this->ptr.lock(); };

    //
    inline decltype(auto) assign(wrap_weak_ptr<T> const& ref) { this->ptr = ref.lock(); return *this; };
    inline decltype(auto) assign(std::weak_ptr<T> const& ref) { this->ptr = ref.lock(); return *this; };
    inline decltype(auto) assign(std::shared_ptr<T> const& ref) { this->ptr = ref; return *this; };
    inline decltype(auto) assign(T const& ref) { if (this->ptr.expired()) { this->ptr = std::make_shared<T>(ref); } else { (*this->ptr) = ref; }; return *this; };
    inline decltype(auto) assign(T && ref) { if (this->ptr.expired()) { this->ptr = std::make_shared<T>(ref); } else { (*this->ptr) = ref; }; return *this; };
    inline decltype(auto) assign(T & ref) { if (this->ptr.expired()) { this->ptr = std::shared_ptr<T>(&ref); } else { (*this->ptr) = ref; }; return *this; };

    //
    inline decltype(auto) operator=(wrap_weak_ptr<T> const& ref) { return this->assign(ref); };
    inline decltype(auto) operator=(std::weak_ptr<T> const& ref) { return this->assign(ref); };
    inline decltype(auto) operator=(std::shared_ptr<T> const& ref) { return this->assign(ref); };
    inline decltype(auto) operator=(T const& ref) { return this->assign(ref); };
    inline decltype(auto) operator=(T && ref) { return this->assign(ref); };
    inline decltype(auto) operator=(T & ref) { return this->assign(ref); };

    //
    inline operator St&() { return this->ptr; };
    inline operator St const&() const { return this->ptr; };

    //
    inline operator std::shared_ptr<T>() { return this->ptr.lock(); };
    inline operator std::shared_ptr<T>() const { return this->ptr.lock(); };

    //
    inline operator T& () { return this->ref(); };
    inline operator T const& () const { return this->ref(); };

    //
    inline operator T* () { return this->get(); };
    inline operator T const* () const { return this->get(); };

    //
    inline auto& operator*() { return this->ref(); };
    inline auto& operator*() const { return this->ref(); };

    //
    inline decltype(auto) operator->() { return this->ptr.lock().get(); };
    inline decltype(auto) operator->() const { return this->ptr.lock().get(); };

    // proxy...
    inline decltype(auto) operator[](uintptr_t const& index) const { return (*this->ptr.lock())[index]; };
    inline decltype(auto) operator[](uint32_t const& index) const { return (*this->ptr.lock())[index]; };

    //
    //inline decltype(auto) operator&() { return this->get(); };
    //inline decltype(auto) operator&() const { return this->get(); };
  };


  //
  template<class T = void_t> class wrap_shared_ptr_;
  template<class T = void_t> using wrap_shared_ptr = wrap_shared_ptr_<decay_t<T>>;

  // 
  template<class T>
  class wrap_shared_ptr_ {
  public:
  protected:
    // TODO: atomic support
    using St = std::shared_ptr<T>;
    St ptr = {};

  public:
    // 
    inline wrap_shared_ptr_(wrap_weak_ptr<T> const& shp) : ptr(shp.lock()) {};
    inline wrap_shared_ptr_(wrap_shared_ptr<T> const& shp) : ptr(shp) {};

    //
    inline wrap_shared_ptr_(St const& shp = {}) : ptr(shp) {};
    inline wrap_shared_ptr_(std::weak_ptr<T> const& shp) : ptr(shp.lock()) {};

    //
    inline wrap_shared_ptr_(T const& shp) : ptr(std::make_shared<T>(shp)) {};
    inline wrap_shared_ptr_(T && shp) : ptr(std::make_shared<T>(shp)) {};
    inline wrap_shared_ptr_(T & shp) : ptr(std::shared_ptr<T>(&shp)) {};
    
    //
    inline wrap_shared_ptr_(T* shp) : ptr(std::shared_ptr<T>(shp)) {};
    inline wrap_shared_ptr_(T const* shp) : ptr(std::shared_ptr<T>(shp)) {};

    //
    inline decltype(auto) weak() { return wrap_weak_ptr(this->ptr); };
    inline decltype(auto) weak() const { return wrap_weak_ptr(this->ptr); };

    //
    inline operator std::weak_ptr<T>() { return wrap_weak_ptr(this->ptr); };
    inline operator std::weak_ptr<T>() const { return wrap_weak_ptr(this->ptr); };

    //
    inline operator wrap_weak_ptr<T>() { return wrap_weak_ptr(this->ptr); };
    inline operator wrap_weak_ptr<T>() const { return wrap_weak_ptr(this->ptr); };

    //
    inline operator bool() const { return !!this->ptr; };

    //
    inline decltype(auto) shared() { return this->ptr; };
    inline decltype(auto) shared() const { return this->ptr; };

    //
    inline decltype(auto) get() { return this->ptr.get(); };
    inline decltype(auto) get() const { return this->ptr.get(); };

    //
    inline auto& ref() { return *this->ptr; };
    inline auto& ref() const { return *this->ptr; };

    //
    inline auto& value() { return *this->ptr; };
    inline auto& value() const { return *this->ptr; };

    //
    inline decltype(auto) assign(St const& ref) { this->ptr = ref; return *this; };
    inline decltype(auto) assign(wrap_shared_ptr<T> const& ref) { this->ptr = ref.shared(); return *this; };
    inline decltype(auto) assign(std::weak_ptr<T> const& ref) { this->ptr = ref.lock(); return *this; };
    inline decltype(auto) assign(wrap_weak_ptr<T> const& ref) { this->ptr = ref.lock(); return *this; };

    //
    inline decltype(auto) assign(T const& ref) { if (!this->ptr) { this->ptr = std::make_shared<T>(ref); } else { (*this->ptr) = ref; }; return *this; };
    inline decltype(auto) assign(T && ref) { if (!this->ptr) { this->ptr = std::make_shared<T>(ref); } else { (*this->ptr) = ref; }; return *this; };
    inline decltype(auto) assign(T & ref) { if (!this->ptr) { this->ptr = std::shared_ptr<T>(&ref); } else { (*this->ptr) = ref; }; return *this; };

    //
    inline decltype(auto) assign(T const* ref) { if (!this->ptr) { this->ptr = std::shared_ptr<T>(ref); } else { (*this->ptr) = *ref; }; return *this; };
    inline decltype(auto) assign(T * ref) { if (!this->ptr) { this->ptr = std::shared_ptr<T>(ref); } else { (*this->ptr) = *ref; }; return *this; };

    // TODO: support other types
    inline decltype(auto) exchange(St const& ref) { auto old = this->ptr; this->ptr = ref; return old; };
    inline decltype(auto) exchange(wrap_shared_ptr<T> const& ref) { auto old = this->ptr; this->ptr = ref.shared(); return old; };

    //
    inline decltype(auto) operator=(St const& ref) { return this->assign(ref); };
    inline decltype(auto) operator=(wrap_shared_ptr<T> const& ref) { return this->assign(ref); };
    inline decltype(auto) operator=(std::weak_ptr<T> const& ref) { return this->assign(ref); };
    inline decltype(auto) operator=(wrap_weak_ptr<T> const& ref) { return this->assign(ref); };

    //
    inline decltype(auto) operator=(T const& ref) { return this->assign(std::move(ref)); };
    inline decltype(auto) operator=(T && ref) { return this->assign(std::move(ref)); };
    inline decltype(auto) operator=(T & ref) { return this->assign(std::move(ref)); };

    //
    inline decltype(auto) operator=(T* ref) { return this->assign(ref); };
    inline decltype(auto) operator=(T const* ref) { return this->assign(ref); };

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
    inline auto& operator*() { return this->ref(); };
    inline auto& operator*() const { return this->ref(); };

    //
    inline decltype(auto) operator->() { return this->ptr.get(); };
    inline decltype(auto) operator->() const { return this->ptr.get(); };

    // proxy...
    inline decltype(auto) operator[](uintptr_t const& index) const { return (*this->ptr)[index]; };
    inline decltype(auto) operator[](uint32_t const& index) const { return (*this->ptr)[index]; };

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
  template<class T = void_t> class const_wrap_arg_;
  template<class T = void_t> using const_wrap_arg = const_wrap_arg_<decay_t<T>>;

  // 
  template<class T>
  class const_wrap_arg_ {
  protected:
    //using T = std::remove_cv_t<std::decay_t<std::remove_cv_t<T>>>;
    T const* ptr = nullptr;
    std::optional<T> temp = {};

  public:
    const_wrap_arg_() {};
    const_wrap_arg_(T&& rvalue) { temp = std::move(rvalue), ptr = &temp.value(); };
    //const_wrap_arg_(T const& lvalue) : ptr(&lvalue) { temp = std::move(lvalue), const_cast<T*&>(ptr) = &temp.value(); }; // losing context
    const_wrap_arg_(T const& lvalue) : ptr(&lvalue) {};
    const_wrap_arg_(T const* pcvalue) : ptr(pcvalue) {};
    const_wrap_arg_(const_wrap_arg<T> const& wvalue) : ptr(wvalue.get()) { this->temp = wvalue.temp; ptr = ((!!this->temp) ? (&const_cast<T&>(this->temp.value())) : (const_cast<T*>(wvalue.get()))); };
    const_wrap_arg_(wrap_ptr<T> const& wvalue) : ptr(wvalue.get()) {};
    const_wrap_arg_(std::optional<T> const& lcvalue) : ptr(lcvalue ? &lcvalue.value() : nullptr) {};
    const_wrap_arg_(wrap_shared_ptr<T> const& wvalue) : ptr(wvalue.get()) {};

    //
    inline operator bool() const { return !!ptr; };

    // # isn't helping to avoid internal compiler error
    inline operator std::optional<T>() const { return this->optional(); };
    inline std::optional<T> optional() const { return ptr ? std::optional<T>(*ptr) : std::optional<T>{}; };

    //
    inline operator T const* () const { return this->get(); };
    inline operator T const& () const { return this->ref(); };

    //
    //inline operator T* () { return this->get(); };
    //inline operator T& () { return this->ref(); };

    //
    inline decltype(auto) operator=(std::optional<T> const& ref) { ptr = ref ? &ref.value() : nullptr; return *this; };
    inline decltype(auto) operator=(T&& ref) { temp = std::move(ref), const_cast<T*&>(ptr) = &temp.value(); return *this; };
    inline decltype(auto) operator=(T const& ref) { ptr = &ref; return *this; };
    inline decltype(auto) operator=(T const* ref) { ptr = ref; return *this; };
    inline decltype(auto) operator=(const_wrap_arg<T> const& wvalue) { temp = wvalue.temp; const_cast<T*&>(ptr) = temp ? &temp.value() : const_cast<T*&>(wvalue.ptr); return *this; };
    inline decltype(auto) operator=(wrap_ptr<T> const& ref) { ptr = ref.get(); return *this; };
    inline decltype(auto) operator=(wrap_shared_ptr<T> const& ref) { ptr = ref.get(); return *this; };

    // value alias
    //inline auto& value() { return *this->get(); };
    inline auto& value() const { return *this->get(); };

    // 
    inline decltype(auto) get() const { return this->ptr; };
    //inline decltype(auto) get() { return const_cast<T*&>(this->ptr); };

    //
    inline auto& ref() const { return *this->get(); };
    //inline auto& ref() { return *this->get(); };

    // accessing operator
    //inline auto& operator *() { return this->ref(); };
    inline auto& operator *() const { return this->ref(); };

    // const accessing operator
    //inline decltype(auto) operator ->() { return this->get(); };
    inline decltype(auto) operator ->() const { return this->get(); };

    // because it's reference, pointer must got directly...
    //inline auto& operator&() { return this->ref(); };
    inline auto& operator&() const { return this->ref(); };

    // proxy...
    inline decltype(auto) operator[](uintptr_t const& index) const { return (*this->ptr)[index]; };
    inline decltype(auto) operator[](uint32_t const& index) const { return (*this->ptr)[index]; };
  };

  //
  template<class T> using carg = const_wrap_arg<T>;
  template<class T> using obj = wrap_shared_ptr<T>;
  template<class T> using object = wrap_shared_ptr<T>;
  template<class T> using wobj = wrap_weak_ptr<T>;
  template<class T> using wobject = wrap_weak_ptr<T>;


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
  template<class Ts = void_t, class T = std::decay_t<Ts>>
  inline decltype(auto) pointer(const_wrap_arg<T> const& ref) {
    T* pt = const_cast<T*>(ref ? (&ref.value()) : nullptr);
    return wrap_ptr<T>(pt);
  };

};

#endif
