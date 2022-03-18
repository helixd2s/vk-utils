#pragma once 

// 
#include "./core.hpp"

// 
namespace cpp21 {

  /*
      // What is intrusive self-copy pointer? I tell about it later...
  */

  // E is extension in before pointer
  template<class E, template<class Es> class Vw> // when extension is needed
  class self_copy_intrusive {
  public: using Vp = Vw<E>; using I = self_copy_intrusive<E, Vw>;
  public:
    E sType = {};
    union {
      Vp pNext; // for Vulkan API
      Vp ptr = nullptr;
    };
    size_t size = 0ull;

  public:
    inline ~self_copy_intrusive() {
      this->free(this->ptr);
    };

    inline self_copy_intrusive(Vp ptr = nullptr, size_t const& size = 0ull) : ptr(ptr ? ptr : this->calloc(size,1ull)), size(size)
    {};

    inline self_copy_intrusive(I const& intrusive) {
      this->assign(intrusive);
    };

  public:
    virtual void memcpy(void* _to, void const* _from, size_t _size) {
      ::memcpy(_to, _from, _size);
    };

    virtual void free(void* _memory) {
      ::free(_memory);
    };

    virtual void* malloc(size_t size) {
      return ::calloc(size,1ull);
    };

    virtual std::type_info& type_info() const {
      return typeid(E);
    };
  public:
    //
    template<class T = void_t> inline decltype(auto) get_t() { return reinterpret_cast<T*>(ptr); };
    template<class T = void_t> inline decltype(auto) get_t() const { return reinterpret_cast<T const*>(ptr); };

    //
    template<class T = void_t> inline decltype(auto) ref_t() { return *this->get<T>(); };
    template<class T = void_t> inline decltype(auto) ref_t() const { return *this->get<T>(); };

    //
    template<class T = void_t> inline decltype(auto) value_t() { return this->ref(); };
    template<class T = void_t> inline decltype(auto) value_t() const { return this->ref(); };

    // 
    inline decltype(auto) assign(auto const& ref) { using T = std::decay_t<decltype(ref)>; if (!this->ptr) { this->ptr = new T; }; (*this->ptr) = ref; return *this; };
    inline decltype(auto) assign(auto const* ptr = nullptr) { using T = std::decay_t<decltype(ptr)>; if (!this->ptr) { this->ptr = new T; }; (*this->ptr) = *ptr; return *this; };

    // 
    inline decltype(auto) operator=(auto& ref) { return this->assign(ref); };
    inline decltype(auto) operator=(auto* ptr) { return this->assign(ptr); };

    // 
    inline decltype(auto) assign(I const& intrusive) {
      if (this->type_info() == intrusive.type_info()) { // if type are equal...
        if (!this->ptr) { this->ptr = (void_t*)this->calloc(this->size = intrusive.size, 1ull); };
        this->memcpy(this->ptr, intrusive.ptr, intrusive.size);
      };
      return *this;
    };
    inline decltype(auto) operator=(I const& intrusive) { return this->assign(intrusive); };

    // proxy...
    //inline decltype(auto) operator[](uintptr_t const& index) { return (*this->ptr)[index]; };
    //inline decltype(auto) operator[](uintptr_t const& index) const { return (*this->ptr)[index]; };
  };


  //
  template<class T, template<class Es> class Iw>
  class self_copy_intrusive_t : public Iw<void_t> {
  public:
    using I = Iw<void_t>;

    // if changable
    inline self_copy_intrusive_t(T& ref, size_t const& size = sizeof(T)) : self_copy_intrusive(reinterpret_cast<I::Vp>(&ref), size ? size : sizeof(T)) {  };
    inline self_copy_intrusive_t(T* ptr, size_t const& size = sizeof(T)) : self_copy_intrusive(reinterpret_cast<I::Vp>(ptr), size ? size : sizeof(T)) {  };

    // if constants
    inline self_copy_intrusive_t(T const& ref, size_t const& size = sizeof(T)) { *this = ref; };
    inline self_copy_intrusive_t(T const* ptr, size_t const& size = sizeof(T)) { *this = ptr; };

    // 
    virtual void memcpy(void* _to, void const* _from, size_t _size) override {
      _memcpy<T, T>(_to, _from, _size);
    };

    // 
    virtual void free(void* _memory) override {
      _free<T>(_memory);
    };

    // 
    virtual void* malloc(size_t _size) override {
      return _malloc<T>(_size);
    };

    // 
    virtual std::type_info& type_info() const override {
      return typeid(T);
    };

    //
    // proxy...
    inline decltype(auto) operator[](uintptr_t const& index) { return this->get(); };
    inline decltype(auto) operator[](uintptr_t const& index) const { return this->get(); };

    //
    template<class Ts = T> inline decltype(auto) get() { return this->get_t<Ts>(); };
    template<class Ts = T> inline decltype(auto) get() const { return this->get_t<Ts>(); };

    //
    template<class Ts = T> inline decltype(auto) ref() { return *this->get_t<Ts>(); };
    template<class Ts = T> inline decltype(auto) ref() const { return *this->get_t<Ts>(); };

    //
    template<class Ts = T> inline decltype(auto) value() { return this->ref(); };
    template<class Ts = T> inline decltype(auto) value() const { return this->ref(); };

    //
    inline decltype(auto) assign(T const& ref) { I::assign(ref); return *this; };
    inline decltype(auto) assign(T const* ptr = nullptr) { I::assign(ptr); return *this; };

    // 
    inline decltype(auto) operator=(T const& ref) { return this->assign(ref); };
    inline decltype(auto) operator=(T const* ptr) { return this->assign(ptr); };

    //
    inline operator T& () { return this->ref(); };
    inline operator T const& () const { return this->ref(); };

    //
    inline operator T* () { return this->get(); };
    inline operator T const* () const { return this->get(); };

    //
    inline operator I& () { return dynamic_cast<I&>(*this); };
    inline operator I const& () const { return dynamic_cast<I const&>(*this); };
  };



  /*
      // What is self-copy pointer? This is two-level, with nested intrusive class-pointer.
      // Can self copy even with changed type to unknown, but used assign operation.
      // Size of is 8...
  */

  // 
  template<class T, template<class Ts> class Iw>
  class self_copy_ptr {
  public: using I = Iw<T>;
  protected:
    wrap_ptr<I> intrusive = {};

  public:
    // 
    inline self_copy_ptr(I* intrusive = nullptr) : intrusive(intrusive) {};
    inline self_copy_ptr(I& intrusive) : intrusive(&intrusive) {};

    // 
    inline self_copy_ptr(T* ptr = nullptr) : intrusive(new I(ptr)) { };
    inline self_copy_ptr(T& ptr) : intrusive(new I(&ptr)) { };

    //
    inline ~self_copy_ptr() { delete this->intrusive; }

    // 
    inline decltype(auto) type_info() const {
      return this->intrusive->type_info();
    };

    //
    inline decltype(auto) get() { this->intrusive->get_t<T>(); };
    inline decltype(auto) get() const { this->intrusive->get_t<T>(); };

    //
    inline decltype(auto) ref() { this->intrusive->ref_t<T>(); };
    inline decltype(auto) ref() const { this->intrusive->ref_t<T>(); };

    //
    inline decltype(auto) value() { this->intrusive->value_t<T>(); };
    inline decltype(auto) value() const { this->intrusive->value_t<T>(); };

    //
    inline decltype(auto) assign(T const& ref) { this->intrusive->assign(ref); return *this; };
    inline decltype(auto) assign(T const* ptr = nullptr) { this->intrusive->assign(ptr); return *this; };

    // 
    inline decltype(auto) operator=(T const& ref) { return this->assign(ref); };
    inline decltype(auto) operator=(T const* ptr) { return this->assign(ptr); };

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
    inline decltype(auto) operator->() { return this->get(); };
    inline decltype(auto) operator->() const { return this->get(); };

    //
    inline decltype(auto) operator&() { return this->get(); };
    inline decltype(auto) operator&() const { return this->get(); };
  };

  // 
  template<class T = void_t, template<class Ts = T> class Iw = self_copy_intrusive_t>
  class enable_self_copy_from_this : public Iw<T> {
  public: using I = Iw<T>; using Ie = enable_self_copy_from_this<T, Iw>;

        // 
        inline enable_self_copy_from_this() : I(reinterpret_cast<T*>(this), sizeof(T)) {};

        // 
        inline decltype(auto) self_copy_from_this() {
          reinterpret_cast<T*>(this->ptr) = reinterpret_cast<T*>(this); // for any case
          return self_copy_ptr<T, Iw>(*this);
        };

        // 
        inline decltype(auto) self_copy_from_this() const {
          const_cast<T*>(reinterpret_cast<T const*>(this->ptr)) = const_cast<T*>(reinterpret_cast<T const*>(this)); // for any case
          return self_copy_ptr<const T, Iw>(*this);
        };
  };

};
