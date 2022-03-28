#pragma once 

// 
#ifdef __cplusplus
#include "./core.hpp"

// 
namespace cpp21 {


  // Prefer Owner with Shared PTR!
  template<class T = uint8_t>
  class uni_ptr {
  protected: //using T = uint8_t;
    std::shared_ptr<T> shared = {};
    optional_ref<T> regular = {};

  public:
    long size = sizeof(T);
    void* owner = nullptr;
    //T storage = {};

  public: friend uni_ptr<T>; // 
        inline uni_ptr() {};
        //inline uni_ptr(uni_ptr<T> const& ptr) : shared(ptr.shared), regular(opt_ref(*ptr.regular)), size(ptr.size), owner(ptr.owner) { 
        //    if (shared && !owner) { owner = &this->shared; }; // JavaCPP direct shared_ptr
        //};
        inline uni_ptr(std::shared_ptr<T> const& shared) : shared(shared), regular(opt_ref(*shared)) {
          if (shared && !owner) { owner = &this->shared; }; // JavaCPP direct shared_ptr
        };
        //inline uni_ptr(T* ptr) : regular(opt_ref(*ptr)) {};
        inline uni_ptr(T& ptr) : regular(opt_ref(ptr)) {};  // for argument passing
        inline uni_ptr(T* ptr, long size = sizeof(T), void* owner = nullptr) : regular(opt_ref(*ptr)), size(size), owner(owner) {
          shared = owner != NULL && owner != ptr ? *(std::shared_ptr<T>*)owner : std::shared_ptr<T>(ptr);
        };
        inline decltype(auto) assign(T* ptr, int size = sizeof(T), void* owner = nullptr) {
          this->regular = opt_ref(*ptr);
          this->size = size;
          this->owner = owner;
          this->shared = owner != NULL && owner != ptr ? *(std::shared_ptr<T>*)owner : std::shared_ptr<T>(ptr);
          return this;
        };
        static inline void deallocate(void* owner) {
          if (owner) { delete (std::shared_ptr<T>*)owner; }; // only when shared available
        };

        // 
        inline decltype(auto) operator= (T* ptr) { regular = opt_ref(*ptr); return this; };
        inline decltype(auto) operator= (T& ptr) { regular = opt_ref(ptr); return this; }; // for argument passing
        inline decltype(auto) operator= (std::shared_ptr<T> const& ptr) {
          shared = ptr, regular = opt_ref(*ptr);
          if (shared && !owner) { owner = &this->shared; }; // JavaCPP direct shared_ptr
          return this;
        };
        //virtual inline uni_ptr* operator= (const uni_ptr<T>& ptr) {
        //    T& ref = *ptr.regular;
        //    shared = ptr.shared, regular = opt_ref(ref), owner = ptr.owner, size = ptr.size;
        //    if (shared && !owner) { owner = &this->shared; }; // JavaCPP direct shared_ptr
        //    return this;
        //};

        // 
        template<class M = T> inline decltype(auto) dyn_cast() const { T& r = *regular; return shared ? uni_ptr<M>(std::dynamic_pointer_cast<M>(shared)) : uni_ptr<M>(dynamic_cast<M&>(r)); };
        template<class M = T> inline decltype(auto) rip_cast() const { T& r = *regular; return shared ? uni_ptr<M>(std::reinterpret_pointer_cast<M>(shared)) : uni_ptr<M>(reinterpret_cast<M&>(r)); };

        // 
        //template<class... A>
        //uni_ptr<T>(A... args) : shared(std::make_shared<T>(args...)) {};

        // TODO: resolve, should or not save a pointer or data copy?
        inline decltype(auto) get_shared() { return (this->shared = (this->shared ? this->shared : std::shared_ptr<T>(get_ptr()))); };
        inline decltype(auto) get_shared() const { return (this->shared ? this->shared : std::shared_ptr<T>(const_cast<T*>(get_ptr()))); };

        // 
        inline decltype(auto) get_ptr() {
          T& r = *regular;
          if (shared && (owner == NULL || owner == &r)) {
            owner = new std::shared_ptr<T>(shared);
          };
          return (T*)((void*)(shared ? &(*shared) : &r));
        };

        // 
        inline decltype(auto) get_ptr() const {
          const T& r = *regular;
          return (T*)((void*)(shared ? &(*shared) : &r));
        };

        // 
        inline bool has() { return regular && shared; };
        inline bool has() const { return regular && shared; };

        //
        inline decltype(auto) get() { return get_ptr(); };
        inline decltype(auto) get() const { return get_ptr(); };

        // 
        inline decltype(auto) ptr() { return get_ptr(); };
        inline decltype(auto) ptr() const { return get_ptr(); };

        // 
        inline decltype(auto) ref() { return *regular; };
        inline decltype(auto) ref() const { return *regular; };

        // experimental
        inline operator T& () { return ref(); };
        inline operator T const& () const { return ref(); };

        // 
        inline operator T* () { return ptr(); };
        inline operator T const* () const { return ptr(); };

        // 
        inline operator std::shared_ptr<T>() { return get_shared(); };
        inline operator const std::shared_ptr<T>() const { return get_shared(); };

        // 
        inline decltype(auto) operator->() { return get_ptr(); };
        inline decltype(auto) operator->() const { return get_ptr(); };

        // 
        inline decltype(auto) operator*() { return *get_ptr(); };
        inline decltype(auto) operator*() const { return *get_ptr(); };
  };


  // 
  template<class T = uint8_t>
  class uni_arg {
  protected:
    //optional_ref<T> storage = {};
    std::optional<T> storage = std::nullopt_t;

  public: // 
    inline uni_arg() {};
    inline uni_arg(T const& t) { storage = t; };
    inline uni_arg(T const* t) { if (t) { storage = *t; }; };
    inline uni_arg(uni_arg<T> const& a) : storage(*a) {};
    explicit inline uni_arg(uni_ptr<T> const& p) : storage(*p) {}; // UnUsual and Vain

    //
    inline uni_arg<T>& operator= (T const& ptr) { storage = ptr; return *this; };
    inline uni_arg<T>& operator= (T const* ptr) { if (ptr) { storage = *ptr; }; return *this; };
    inline uni_arg<T>& operator= (uni_arg<T> t) { if (t.has()) { storage = t.ref(); }; return *this; };
    inline uni_arg<T>& operator= (uni_ptr<T> p) { if (p.has()) { storage = *p.ptr(); }; return *this; };

    // experimental
    inline operator T& () { return ref(); };
    inline operator T const& () const { return ref(); };

    // 
    inline operator T* () { return ptr(); };
    inline operator T const* () const { return ptr(); };

    // 
    inline operator uni_ptr<T>() { handle(has()); return *storage; };
    inline operator uni_ptr<const T>() const { handle(has()); return *storage; };

    // 
    inline decltype(auto) has_value() const { return storage.has_value(); };
    inline decltype(auto) has() const { return this->has_value(); };

    // 
    inline decltype(auto) ptr() { handle(has()); return (T*)((void*)(&(*storage))); };
    inline decltype(auto) ptr() const { handle(has()); return (T*)((void*)(&(*storage))); };

    //
    template<class M = T> inline decltype(auto) ptr() { handle(has()); return &reinterpret_cast<M&>(this->ref()); };
    template<class M = T> inline decltype(auto) ptr() const { handle(has()); return &reinterpret_cast<M const&>(this->ref()); };

    // 
    inline decltype(auto) ref() { handle(has()); return *storage; };
    inline decltype(auto) ref() const { handle(has()); return *storage; };

    // 
    inline decltype(auto) operator->() { return ptr(); };
    inline decltype(auto) operator->() const { return ptr(); };

    //
    inline decltype(auto) operator*() { return ref(); };
    inline decltype(auto) operator*() const { return ref(); };
  };


  // Bi-Directional Conversion
  template<class T = uint8_t, class B = char8_t>
  class uni_dir {
  protected:
    std::optional<T> storage = std::nullopt;
  public: // 
    inline uni_dir() {};
    inline uni_dir(T const& t) : storage(t) {};
    inline uni_dir(B const& t) : storage(reinterpret_cast<const T&>(t)) {};
    inline uni_dir(T const* t) : storage(*t) {};
    inline uni_dir(B const* t) : storage(reinterpret_cast<const T&>(*t)) {};

    // 
    inline uni_dir(uni_ptr<T> const& p) : storage(*p) {}; // UnUsual and Vain
    inline uni_dir(uni_ptr<B> const& p) : storage(reinterpret_cast<T&>(*p)) {}; // UnUsual and Vain
    inline uni_dir(uni_arg<T> const& a) : storage(*a) {};
    inline uni_dir(uni_arg<B> const& a) : storage(reinterpret_cast<T&>(*a)) {};

    //
    inline decltype(auto) operator= (T const& ptr) { storage = ptr; return *this; };
    inline decltype(auto) operator= (T const* ptr) { storage = *ptr; return *this; };
    inline decltype(auto) operator= (uni_arg<T> t) { storage = t.ref(); return *this; };
    inline decltype(auto) operator= (uni_ptr<T> p) { storage = *p.ptr(); return *this; };

    //
    inline decltype(auto) operator= (B const& ptr) { storage = reinterpret_cast<T const&>(ptr); return *this; };
    inline decltype(auto) operator= (B const* ptr) { storage = reinterpret_cast<T const&>(*ptr); return *this; };
    inline decltype(auto) operator= (uni_arg<B> t) { storage = reinterpret_cast<T&>(t.ref()); return *this; };
    inline decltype(auto) operator= (uni_ptr<B> p) { storage = reinterpret_cast<T&>(*p.ptr()); return *this; };

    // 
    inline operator T& () { return ref(); };
    inline operator T const& () const { return ref(); };

    // 
    inline operator T* () { return ptr(); };
    inline operator T const* () const { return ptr(); };

    // 
    inline operator B& () { return reinterpret_cast<B&>(ref()); };
    inline operator B const& () const { return reinterpret_cast<B const&>(ref()); };

    // 
    inline operator B* () { return reinterpret_cast<B*>(ptr()); };
    inline operator B const* () const { return reinterpret_cast<B const*>(ptr()); };

    // 
    inline operator uni_ptr<T>() { handle(has()); return *storage; };
    inline operator const uni_ptr<T>() const { handle(has()); return *storage; };

    // 
    inline decltype(auto) has_value() const { return storage.has_value(); };
    inline decltype(auto) has() const { return this->has_value(); };

    // 
    inline decltype(auto) ptr() { handle(has()); return &(*storage); };
    inline decltype(auto) ptr() const { handle(has()); return &(*storage); };

    //
    template<class M = T> inline decltype(auto) ptr() { handle(has()); return &reinterpret_cast<M&>(this->ref()); };
    template<class M = T> inline decltype(auto) ptr() const { handle(has()); return &reinterpret_cast<M const&>(this->ref()); };

    // 
    inline decltype(auto) ref() { handle(has()); return *storage; };
    inline decltype(auto) ref() const { handle(has()); return *storage; };

    // 
    inline decltype(auto) operator->() { return this->ptr(); };
    inline decltype(auto) operator->() const { return this->ptr(); };

    //
    inline decltype(auto) operator*() { return ref(); };
    inline decltype(auto) operator*() const { return ref(); };
  };


};
#endif
