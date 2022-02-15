#pragma once

// 
#include <memory>
#include <vector>
#include <map>
#include <future>
#include <iostream>
#include <optional>
#include <coroutine>
#include <tuple>
#include <type_traits>

// 
#ifdef VKU_ENABLE_TYPE_SAFE
#include <type_safe/optional_ref.hpp>
#include <type_safe/reference.hpp>
#include <type_safe/integer.hpp>
#endif

// 
#ifdef VKU_ENABLE_INTERVAL
#include <interval-tree/interval_tree.hpp>
#endif

// 
namespace stm {

    // make short alias
#ifdef VKU_ENABLE_TYPE_SAFE
    namespace ts {
        using namespace type_safe;
    };
#endif

    /* 
        // What is "void_t"? This is specific type, replacement for C void for most case. Less conflicts, more compatible, more functional. But sometime C++ may make this type 1-byte sized.
        // Mostly used with pointers and with address spaces, and operates with it.
    */


    // aggregate cache
    inline auto* cache = new unsigned char[256u*256u];

    // 
    inline decltype(auto) aggregate(auto str){
        using T = std::decay_t<decltype(str)>;
        memcpy(cache, &str, sizeof(T));
        return reinterpret_cast<T&>(*cache);
    };

    // 
    inline decltype(auto) aggregate(auto str, auto& cache_pointer){
        using T = std::decay_t<decltype(str)>;
        memcpy(cache, &str, sizeof(T));
        return reinterpret_cast<T&>(*cache);
    };

    // 
    inline decltype(auto) unlock32(auto& cache) {
        return reinterpret_cast<uint32_t&>(cache);
    };

    // 
    inline decltype(auto) zero32(auto& cache) {
        return (reinterpret_cast<uint32_t& >(cache) = 0u);
    };

    // 
    inline decltype(auto) unlock64(auto& cache){
        return reinterpret_cast<uint64_t&>(cache);
    };

    // 
    inline decltype(auto) zero64(auto& cache){
        return (reinterpret_cast<uint64_t& >(cache) = 0u);
    };

    //
    enum default_t : uint32_t {

    };

    // only for construct vulkan structures
    //template<class T>
    inline decltype(auto) copy_as_shared(auto const* data) {
        using T = std::decay_t<decltype(data)>;
        auto shared = std::shared_ptr<T>((T*)malloc(sizeof(T)), free);
        memcpy(shared.get(), data, sizeof(T));
        //*shared = data; // what if structure can self-copy?
        return shared;
    };

    // 
    inline decltype(auto) copy_as_shared(auto const& data) {
        return copy_as_shared(&data);
    };

    //
    using localptr_t = uintptr_t;


    //#ifdef USE_VULKAN
    static inline decltype(auto) sgn(auto const& val) { using T = std::decay_t<decltype(val)>; return (T(0) < val) - (val < T(0)); }
    static inline decltype(auto) tiled(auto const& sz, auto const& gmaxtile) {
        // return (int32_t)ceil((double)sz / (double)gmaxtile);
        return sz <= 0 ? 0 : (sz / gmaxtile + sgn(sz % gmaxtile));
    };

    // 
    template <class T> static inline decltype(auto) strided(size_t const& size = 1u) { return sizeof(T) * size; };
    template <class T> static inline decltype(auto) makeVector(T const* ptr, size_t const& size = 1) { std::vector<T>v(size); memcpy(v.data(), ptr, strided<T>(size)); return v; };

    // 
    template<class T, class Ts = T>
    inline decltype(auto) vector_cast(std::vector<Ts> const& Vy) {
        std::vector<T> V{}; for (auto& v : Vy) { V.push_back(reinterpret_cast<T const&>(v)); }; return std::move(V);
    };

    // 
    inline decltype(auto) shift(auto* data, uintptr_t offset = 0ull) { return reinterpret_cast<decltype(data)>(reinterpret_cast<uintptr_t&>(data) + offset); };
    inline decltype(auto) shift(const auto* data, uintptr_t offset = 0ull) { return reinterpret_cast<decltype(data)>(reinterpret_cast<const uintptr_t&>(data) + offset); };


    //using void_t = uint8_t;
    class void_t { public: 
        //uint8_t : 0;

        // 
        void_t(){};

        //
        decltype(auto) assign(auto const* obj) { using Ts = std::decay_t<decltype(obj)>; memcpy(this, obj, sizeof(Ts)); return reinterpret_cast<Ts&>(*this); };
        decltype(auto) assign(auto const& obj) { return this->assign(&obj); };

        //
        decltype(auto) operator=(auto const& obj) { return this->assign(obj); };
        decltype(auto) operator=(auto const* obj) { return this->assign(obj); };

        // 
        template<class Ts = void_t> decltype(auto) operator->() { return this->get<Ts>(); };
        template<class Ts = void_t> decltype(auto) operator->() const { return this->get<Ts>(); };

        // 
        template<class Ts = void_t> decltype(auto) operator *() { return *this->get<Ts>(); };
        template<class Ts = void_t> decltype(auto) operator *() const { return *this->get<Ts>(); };

        // 
        template<class Ts = void_t> operator Ts&() { return *this->get<Ts>(); };
        template<class Ts = void_t> operator Ts const&() const { return *this->get<Ts>(); };

        // 
        template<class Ts = void_t> operator Ts*&() { return this->get<Ts>(); };
        template<class Ts = void_t> operator Ts* const&() const { return this->get<Ts>(); };

        // 
        template<class Ts = void_t> decltype(auto) get() { return reinterpret_cast<Ts*&>(this); };
        template<class Ts = void_t> decltype(auto) get() const { return reinterpret_cast<Ts* const&>(this); };

        // 
        operator bool() const { return false; };
    };

    // 
//#ifdef TYPE_SAFE_OPTIONAL_REF_HPP_INCLUDED
//    using optional_ref = ts::optional_ref;
//    using opt_ref = ts::opt_ref;
//    using opt_cref = ts::opt_cref;
//#else 
    // own implementation of optional ref


    /* 
        // What is optional ref pointer? This is our implementation optional_ref from type_safe, more functional.
    */

    template<class T = void_t>
    class optional_ref {
    protected:
        T* ptr = nullptr;

    public: 
        // 
#ifdef TYPE_SAFE_OPTIONAL_REF_HPP_INCLUDED
        optional_ref(ts::optional_ref<T> const& ref) : ptr(ref ? ref.value() : nullptr) {};
        optional_ref(ts::optional_ref<T>& ref) : ptr(ref ? ref.value() : nullptr) {};
#endif

        // 
        optional_ref(optional_ref<T> const& ref) : ptr(ref ? ref.value() : nullptr) {};
        optional_ref(optional_ref<T>& ref) : ptr(ref ? ref.value() : nullptr) {};
        optional_ref(T const& ref) : ptr(&ref) {};
        optional_ref(T& ref) : ptr(&ref) {};
        optional_ref() {};

        // 
        operator T&() { return *this->ptr; }
        operator T const&() const { return *this->ptr; }

        //
#ifdef TYPE_SAFE_OPTIONAL_REF_HPP_INCLUDED
        ts::optional_ref<T>() { return ts::opt_ref(*this->ptr); };
        ts::optional_ref<const T>() const { return ts::opt_cref(*this->ptr); };
#endif

        //
#ifdef TYPE_SAFE_OPTIONAL_REF_HPP_INCLUDED
        decltype(auto) operator=(ts::optional_ref<T> const& ref) { ptr = ref ? ref.value() : nullptr; return *this; };
        decltype(auto) operator=(ts::optional_ref<T>& ref) { ptr = ref ? ref.value() : nullptr; return *this; };
#endif

        // assign ref
        decltype(auto) operator=(optional_ref<T> const& ref) { ptr = ref ? ref.value() : nullptr; return *this; };
        decltype(auto) operator=(optional_ref<T>& ref) { ptr = ref ? ref.value() : nullptr; return *this; };
        decltype(auto) operator=(T& ref) { ptr = &ref; return *this; };
        decltype(auto) operator=(T const& ref) { *ptr = ref; return *this; };

        // value alias
        decltype(auto) value() { return *this->ptr; }
        decltype(auto) value() const { return *this->ptr; }

        // accessing operator
        decltype(auto) operator *() { return *this->ptr; };
        decltype(auto) operator *() const { return *this->ptr; };

        // const accessing operator
        decltype(auto) operator ->() { return this->ptr; };
        decltype(auto) operator ->() const { return this->ptr; };

        // check operator
        operator bool() const { return !!this->ptr; };
    };



    /* 
        // What is intrusive self-copy pointer? I tell about it later...
    */

    template<class T = void_t>
    inline decltype(auto) opt_ref(T& ref) {
        return optional_ref<T>(ref);
    };

    template<class T = void_t>
    inline decltype(auto) opt_cref(const T& ref) {
        return optional_ref<const T>(ref);
    };

    //
    template<class E = void_t, class Vp = void_t*>
    class self_copy_intrusive;

    // 
    template<class T = void_t, class I = self_copy_intrusive<T>>
    class self_copy_intrusive_t;

    // 
    template<class T = void_t, class I = self_copy_intrusive_t<T>>
    class self_copy_ptr;

    // E is extension in before pointer
    template<class E = void_t, class Vp = void_t*> // when extension is needed
    class self_copy_intrusive {
    public: using E = E; using Vp = Vp; using I = self_copy_intrusive<E,Vp>;
    public:
        E sType = {};
        union {
            Vp pNext; // for Vulkan API
            Vp ptr = nullptr;
        };
        size_t size = 0ull;

    public:
        ~self_copy_intrusive() {
            this->free(this->ptr);
        };

        self_copy_intrusive(Vp ptr = nullptr, size_t const& size = 0ull) : ptr(ptr ? ptr : this->malloc(size)), size(size)
        {};

        self_copy_intrusive(I const& intrusive) {
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
            return ::malloc(size);
        };

    public: 
        //
        template<class T = void_t> decltype(auto) get() { return reinterpret_cast<T*>(ptr); };
        template<class T = void_t> decltype(auto) get() const { return reinterpret_cast<T const*>(ptr); };

        //
        template<class T = void_t> decltype(auto) ref() { return *this->get<T>(); };
        template<class T = void_t> decltype(auto) ref() const { return *this->get<T>(); };

        //
        template<class T = void_t> decltype(auto) value() { return this->ref(); };
        template<class T = void_t> decltype(auto) value() const { return this->ref(); };

        // 
        decltype(auto) assign(auto const& ref) { if (!this->ptr) { this->ptr = new T; }; (*this->ptr) = ref; return *this; };
        decltype(auto) assign(auto const* ptr = nullptr) { if (!this->ptr) { this->ptr = new T; }; (*this->ptr) = *ptr; return *this; };

        // 
        decltype(auto) operator=(auto& ref) { return this->assign(ref); };
        decltype(auto) operator=(auto* ptr) { return this->assign(ptr); };

        // 
        decltype(auto) assign(I const& intrusive) { if (!this->ptr) { this->ptr = (void_t*)this->malloc(this->size = intrusive.size); }; this->memcpy(this->ptr, intrusive.ptr, intrusive.size); return *this; };
        decltype(auto) operator=(I const& intrusive) { return this->assign(intrusive); };
    };

    //
    template<class T = void>
    void _free(void* ptr_) {
        if (ptr_) { delete reinterpret_cast<T*>(ptr_); };
    };

    //
    template<class Tt = void, class Tf>
    void _memcpy(void* _to, void const* _from, size_t _size) {
        (*reinterpret_cast<Tt*>(to)) = (*reinterpret_cast<Tf const*>(from));
    };

    //
    template<class T>
    void* _malloc(size_t _size) {
        // TODO: correct division
        return (new T[_size/sizeof(T)]);
    };

    //
    template<class T = void_t, class I = self_copy_intrusive<T>>
    class self_copy_intrusive_t : public I {
    public: using T = T; using I = I;

        // if changable
        self_copy_intrusive_t(T& ref, size_t const& size = sizeof(T)) : self_copy_intrusive(reinterpret_cast<I::Vp>(&ref), size ? size : sizeof(T)) {  };
        self_copy_intrusive_t(T* ptr, size_t const& size = sizeof(T)) : self_copy_intrusive(reinterpret_cast<I::Vp>(ptr), size ? size : sizeof(T)) {  };

        // if constants
        self_copy_intrusive_t(T const& ref, size_t const& size = sizeof(T)) { *this = ref; };
        self_copy_intrusive_t(T const* ptr, size_t const& size = sizeof(T)) { *this = ptr; };

        // 
        virtual void memcpy(void* _to, void const* _from, size_t _size) override {
            _memcpy<T,T>(_to,_from,_size);
        };

        // 
        virtual void free(void _memory) override {
            _free<T>(_memory);
        };

        // 
        virtual void* malloc(size_t _size) override {
            return _malloc<T>(_size);
        };

        //
        decltype(auto) get() { return this->get<T>(); };
        decltype(auto) get() const { return this->get<T>(); };

        //
        decltype(auto) ref() { return *this->get(); };
        decltype(auto) ref() const { return *this->get(); };

        //
        decltype(auto) value() { return this->ref(); };
        decltype(auto) value() const { return this->ref(); };

        //
        decltype(auto) assign(T const& ref) { I::assign(ref); return *this; };
        decltype(auto) assign(T const* ptr = nullptr) { I::assign(ptr); return *this; };

        // 
        decltype(auto) operator=(T const& ref) { return this->assign(ref); };
        decltype(auto) operator=(T const* ptr) { return this->assign(ptr); };

        //
        operator T&() { return this->ref(); };
        operator T const&() const { return this->ref(); };

        //
        operator T*() { return this->get(); };
        operator T const*() const { return this->get(); };

        //
        operator I&() { return dynamic_cast<I&>(*this); };
        operator I const&() const { return dynamic_cast<I const&>(*this); };
    };



    /* 
        // What is self-copy pointer? This is two-level, with nested intrusive class-pointer.
        // Can self copy even with changed type to unknown, but used assign operation. 
        // Size of is 8...
    */

    // 
    template<class T = void_t, class I = self_copy_intrusive_t<T>>
    class self_copy_ptr {
    public: using T = T; using I = I;
    protected:
        I* intrusive = {};

    public: 
        // 
        self_copy_ptr(I* intrusive = nullptr) : intrusive(intrusive) {};
        self_copy_ptr(I& intrusive) : intrusive(&intrusive) {};

        // 
        self_copy_ptr(T* ptr = nullptr) : intrusive(new I( ptr )) { };
        self_copy_ptr(T& ptr) : intrusive(new I( &ptr )) { };

        //
        ~self_copy_ptr() { delete this->intrusive; }

        //
        decltype(auto) get() { this->intrusive->get<T>(); };
        decltype(auto) get() const { this->intrusive->get<T>(); };

        //
        decltype(auto) ref() { this->intrusive->ref<T>(); };
        decltype(auto) ref() const { this->intrusive->ref<T>(); };

        //
        decltype(auto) value() { this->intrusive->value<T>(); };
        decltype(auto) value() const { this->intrusive->value<T>(); };

        //
        decltype(auto) assign(T const& ref) { this->intrusive->assign(ref); return *this; };
        decltype(auto) assign(T const* ptr = nullptr) { this->intrusive->assign(ptr); return *this; };

        // 
        decltype(auto) operator=(T const& ref) { return this->assign(ref); };
        decltype(auto) operator=(T const* ptr) { return this->assign(ptr); };

        //
        operator T&() { return this->ref(); };
        operator T const&() const { return this->ref(); };

        //
        operator T*() { return this->get(); };
        operator T const*() const { return this->get(); };

        //
        decltype(auto) operator*() { return this->ref(); };
        decltype(auto) operator*() const { return this->ref(); };

        //
        decltype(auto) operator->() { return this->get(); };
        decltype(auto) operator->() const { return this->get(); };
    };

    // 
    template<class T = void_t, class I = self_copy_intrusive_t<T>>
    class enable_self_copy_from_this : public I {
        public: using T = T; using I = I; using Ie = decltype(*this);

        // 
        enable_self_copy_from_this() : I(reinterpret_cast<T*>(this), sizeof(T)) {};

        // 
        decltype(auto) self_copy_from_this() {
            reinterpret_cast<T*>(this->ptr) = reinterpret_cast<T*>(this); // for any case
            return self_copy_ptr<T, Ie>(*this);
        };

        // 
        decltype(auto) self_copy_from_this() const {
            const_cast<T*>(reinterpret_cast<T const*>(this->ptr)) = const_cast<T*>(reinterpret_cast<T const*>(this)); // for any case
            return self_copy_ptr<const T, Ie>(*this);
        };
    };


    /* 
        // What is wrap_ptr? This is wrapper for C pointers, for avoid some conflicts and problems.
        // There is no specifical features, only pointers compatibility...
        // Size of is 8...
    */

    // 
    template<class T>
    class wrap_ptr {
    protected:
        T* ptr = nullptr;

    public:
        wrap_ptr(T* const& ptr = nullptr) { this->ptr = ptr; };

        // 
        decltype(auto) operator->() { return this->get(); };
        decltype(auto) operator->() const { return this->get(); };

        // 
        decltype(auto) operator *() { return *this->get(); };
        decltype(auto) operator *() const { return *this->get(); };

        // 
        operator T& () { return *this->get(); };
        operator const T& () const { return *this->get(); };

        // 
        operator T*& () { return this->get(); };
        operator T* const& () const { return this->get(); };

        // 
        decltype(auto) ref() { return *this->get(); };
        decltype(auto) ref() const { return *this->get(); };

        // 
        decltype(auto) value() { return *this->get(); };
        decltype(auto) value() const { return *this->get(); };

        //
        decltype(auto) get() { return reinterpret_cast<T*&>(ptr); };
        decltype(auto) get() const { return reinterpret_cast<T* const&>(ptr); };

        // 
        decltype(auto) operator =(T* const& ptr) { this->ptr = ptr; return *this; };

        // 
        template<class W=wrap_ptr<T>>
        decltype(auto) operator =(W const& ptr) { this->ptr = ptr.get(); return *this; };

        // 
        operator bool() const { return !!ptr; };
    };



    /* 
        // What is link? This is self-copy pointer, but without known size for deep operations
        // Usable for type-known classes, methods, structures...
        // Size of only 8 byte, not nested...
    */

    //
    //[[deprecated]]
    class link_void;

    //
    //[[deprecated]]
    template<class T = void_t>
    class link;

    // 
    //[[deprecated]]
    class link_void {
    protected: 
        void_t* ptr = nullptr;

    public: 
        // 
        decltype(auto) assign(void_t const* obj, size_t const& size) { if (!this->ptr) { this->ptr = (void_t*)malloc(size); }; memcpy(this->ptr, obj, size); return *this; };
        decltype(auto) assign(void_t const* obj) { std::cerr << "sorry, but we doesn't know assign size" << std::endl; return *this; };
        decltype(auto) operator=(void_t const* obj) { return this->assign(obj); };

        // 
        link_void() {};
        link_void(void_t const* ptr) { this->assign(ptr); };
        ~link_void() { free(ptr); };

        //
        //decltype(auto) assign(auto const* obj) { using Ts = std::decay(decltype(obj))::type; return reinterpret_cast<link<Ts>&>(this->assign(reinterpret_cast<void_t const*>(obj), sizeof(Ts))); };
        decltype(auto) assign(auto const* obj) {
            using Ts = std::decay_t<decltype(obj)>;
            if (!this->ptr) { this->ptr = new Ts; };
            *reinterpret_cast<Ts*>(this->ptr) = *obj;
            return reinterpret_cast<link<Ts>&>(*this);
        };

        // type aliasing
        decltype(auto) assign(auto const& obj) { return this->assign(&obj); };

        //
        decltype(auto) operator=(auto const& obj) { return this->assign(obj); };
        decltype(auto) operator=(auto const* obj) { return this->assign(obj); };

        // to avoid ambiguous
        template<class Ts = void_t, class Ls = link<Ts>>
        decltype(auto) operator=(Ls const& obj) { return this->assign(obj.get<Ts>()); };

        // 
        template<class Ts = void_t> decltype(auto) operator->() { return this->get(); };
        template<class Ts = void_t> decltype(auto) operator->() const { return this->get(); };

        // 
        template<class Ts = void_t> decltype(auto) operator *() { return *this->get(); };
        template<class Ts = void_t> decltype(auto) operator *() const { return *this->get(); };

        // 
        template<class Ts = void_t> operator Ts&() { return *this->get(); };
        template<class Ts = void_t> operator Ts const&() const { return *this->get(); };

        // 
        template<class Ts = void_t> operator Ts*&() { return this->get(); };
        template<class Ts = void_t> operator Ts* const&() const { return this->get(); };

        // 
        template<class Ts = void_t> decltype(auto) get() { return reinterpret_cast<Ts*&>(this->ptr); };
        template<class Ts = void_t> decltype(auto) get() const { return reinterpret_cast<Ts* const&>(this->ptr); };
    };

    // 
    //[[deprecated]]
    template<class T = void_t>
    class link : public link_void { 

    public: 
        link() : link_void() {};
        link(const void* const& ptr) : link_void(ptr) {};
        link(const T* const& obj) { this->assign(obj); };
        link(const link<T>& obj) { this->assign(obj.get()); };

        // 
        decltype(auto) operator->() { return this->get(); };
        decltype(auto) operator->() const { return this->get(); };

        // 
        decltype(auto) operator *() { return *this->get(); };
        decltype(auto) operator *() const { return *this->get(); };

        // 
        operator T&() { return *this->get(); };
        operator T const&() const { return *this->get(); };

        // 
        operator T*&() { return this->get(); };
        operator T* const&() const { return this->get(); };

        // 
        decltype(auto) get() { return reinterpret_cast<T*&>(this->ptr); };
        decltype(auto) get() const { return reinterpret_cast<T* const&>(this->ptr); };

        //
        decltype(auto) assign(auto const* obj) { return reinterpret_cast<link<std::decay_t<decltype(obj)>>>(link_void::assign(obj)); };
        decltype(auto) assign(auto const& obj) { return reinterpret_cast<link<std::decay_t<decltype(obj)>>>(link_void::assign(obj)); };

        //
        decltype(auto) operator=(auto const* obj) { return this->assign(obj); };
        decltype(auto) operator=(auto const& obj) { return this->assign(obj); };

        // to avoid ambiguous
        template<class Ts = T, class Ls = link<Ts>>
        decltype(auto) operator=(Ls const& obj) { return this->assign(obj.get()); };
    };

    //
    template<class Ts = void_t>
    inline decltype(auto) pointer(optional_ref<Ts> ref) {
        return ref ? (&ref.value()) : nullptr;
    };

    //
    template<class Ts = void_t>
    inline decltype(auto) pointer(optional_ref<const Ts> ref) {
        return ref ? (&ref.value()) : nullptr;
    };

    //
    template<class Ts = void_t>
    inline decltype(auto) pointer(std::optional<Ts>& ref) {
        return ref ? (&ref.value()) : nullptr;
    };

    //
    template<class Ts = void_t>
    inline decltype(auto) pointer(std::optional<Ts> const& ref) {
        return ref ? (&ref.value()) : nullptr;
    };

    //
    template<class Ts = void_t>
    inline decltype(auto) pointer(std::optional<const Ts> const& ref) {
        return ref ? (&ref.value()) : nullptr;
    };

#ifdef TYPE_SAFE_OPTIONAL_REF_HPP_INCLUDED
    //
    template<class Ts = void_t>
    inline decltype(auto) pointer(ts::optional_ref<Ts> ref) {
        return ref ? (&ref.value()) : nullptr;
    };

    //
    template<class Ts = void_t>
    inline decltype(auto) pointer(ts::optional_ref<const Ts> ref) {
        return ref ? (&ref.value()) : nullptr;
    };
#endif
    
    // 
    inline void handle(const bool& valid = false) {
        if (!valid) {
            //std::cerr << "std::optional is wrong or not initialized" << std::endl; assert(valid);
        };
    };

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
        inline decltype(auto) operator= (T& ptr) { regular = opt_ref( ptr); return this; }; // for argument passing
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
        template<class M = T>
        inline decltype(auto) dyn_cast() const { T& r = *regular; return shared ? uni_ptr<M>(std::dynamic_pointer_cast<M>(shared)) : uni_ptr<M>(dynamic_cast<M&>(r)); };
        inline decltype(auto) rip_cast() const { T& r = *regular; return shared ? uni_ptr<M>(std::reinterpret_pointer_cast<M>(shared)) : uni_ptr<M>(reinterpret_cast<M&>(r)); };

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
        inline operator decltype(auto) () { return get_shared(); };
        inline operator decltype(auto) () const { return get_shared(); };

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
        std::optional<T> storage = std::nullopt;
    public: // 
        uni_arg() {};
        uni_arg(T const& t) { storage = t; };
        uni_arg(T const* t) { if (t) { storage = *t; }; };
        uni_arg(uni_ptr<T> const& p) : storage(*p) {}; // UnUsual and Vain
        uni_arg(uni_arg<T> const& a) : storage(*a) {};

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
        uni_dir() {};
        uni_dir(T const& t) : storage(t) {};
        uni_dir(B const& t) : storage(reinterpret_cast<const T&>(t)) {};
        uni_dir(T const* t) : storage(*t) {};
        uni_dir(B const* t) : storage(reinterpret_cast<const T&>(*t)) {};

        // 
        uni_dir(uni_ptr<T> const& p) : storage(*p) {}; // UnUsual and Vain
        uni_dir(uni_ptr<B> const& p) : storage(reinterpret_cast<T&>(*p)) {}; // UnUsual and Vain
        uni_dir(uni_arg<T> const& a) : storage(*a) {};
        uni_dir(uni_arg<B> const& a) : storage(reinterpret_cast<T&>(*a)) {};

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
        inline operator uni_ptr<const T>() const { handle(has()); return *storage; };

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

    //
    template<class T, uint32_t count = 16u>
    class limited_list { public: 
        std::list<T> queue = {};

        limited_list* push_back( T const& e) {
            if (queue.size() >= count) { queue.pop_front(); };
            queue.push_back(e);
            return this;
        };

        T& back() { return queue.back(); };
        T const& back() const { return queue.back(); };

        size_t size() const { return queue.size(); };
    };

    // only for construct vulkan structures
    //template<class T>
    //inline decltype(auto) shared_struct(T const& data) {
    //    auto shared = std::shared_ptr<T>((T*)malloc(sizeof(T)), free);
    //    //memcpy(shared.get(), &data, sizeof(T));
    //    *shared = data; // what if structure can self-copy?
    //    return shared;
    //};

    // for vulkan structs or descriptor set data bucket
    template<class U = uint8_t, class Vc = std::vector<U>>
    class memory_stack {
    protected:
        // we use local pointer memory
        Vc memory = {};

    public: 
        // 
        memory_stack(Vc const& memory = {}) : memory(memory) {
            
        };

        // 
        template<class S = U>
        decltype(auto) push(S const& data) {
            auto last = memory.size();
            memory.resize(last + sizeof(S));
            memcpy(memory.data() + last, &data, sizeof(S));
            return last;
        };

        // 
        template<class S = U>
        decltype(auto) get(localptr_t const& address) {
            return uni_ptr(reinterpret_cast<S*>(memory.data() + address));
        };

        // 
        template<class S = U>
        decltype(auto) get(localptr_t const& address) const {
            return uni_ptr(reinterpret_cast<const S*>(memory.data() + address));
        };
    };

    // for vulkan structs with shared_ptr
    template<class V = void_t, class Vc = std::vector<V>>
    class vector_of_shared {
    protected:
        // we use local pointer memory
        Vc<std::shared_ptr<V>> chain = {};

    public: 
        // 
        vector_of_shared(Vc<std::shared_ptr<V>> const& chain) : chain(chain) {};

        // 
        decltype(auto) push(auto const& data = {}) {
            using T = std::decay_t<decltype(data)>;
            auto last = chain.size();
            chain->push_back(std::reinterpret_pointer_cast<T>(copy_as_shared(data)));
            return last;
        };

        // 
        template<class T = V>
        decltype(auto) push(std::shared_ptr<T> const& data = {}) {
            auto last = chain.size();
            chain->push_back(std::reinterpret_pointer_cast<V>(data));
            return last;
        };

        // 
        template<class T = V>
        decltype(auto) get(uintptr_t const& index = 0u) {
            return uni_ptr(std::reinterpret_pointer_cast<T>(chain[index]));
        };

        // 
        template<class T = V>
        decltype(auto) get(uintptr_t const& index = 0u) const {
            return uni_ptr(std::reinterpret_pointer_cast<T>(chain[index]));
        };

        //
        template<class T = V>
        decltype(auto) push_get(auto const& data = {}) {
            return this->get<T>(this->push(data));
        };
    };

    // for advanced vulkan structs with shared_ptr
    template<class K = uintptr_t, class V = void_t>
    class map_of_shared {
    protected:
        // we use local pointer memory
        std::unordered_map<K, std::shared_ptr<V>> map = {};

    public: 
        map_of_shared(std::unordered_map<K, std::shared_ptr<V>> const& map = {}) : map(map) {};

        // 
        decltype(auto) set(K const& key, auto const& data = {}) {
            using T = std::decay_t<decltype(data)>;
            return uni_ptr(map[key] = std::reinterpret_pointer_cast<T>(copy_as_shared(data)));
        };

        // 
        template<class T = V>
        decltype(auto) set(K const& key, std::shared_ptr<T> const& data = {}) {
            return uni_ptr(map[key] = std::reinterpret_pointer_cast<V>(data));
        };

        // 
        template<class T = V>
        decltype(auto) get(K const& key) {
            return uni_ptr(std::reinterpret_pointer_cast<T>(map.at(key)));
        };

        // 
        template<class T = V>
        decltype(auto) get(K const& key) const {
            return uni_ptr(std::reinterpret_pointer_cast<T>(map.at(key)));
        };
    };

#ifdef VKU_ENABLE_INTERVAL
    // 
    template<class N, class T>
    class interval_map { public: 
        std::unordered_map<N, T> map = {};
        lib_interval_tree::interval_tree_t<N> intervals = {};

    public:
        // 
        decltype(auto) insert( lib_interval_tree::interval<N, lib_interval_tree::closed> const& interval, T const& obj) {
            auto it = intervals.overlap_find({ interval.low(), interval.low() });
            if (map.find(interval.low()) == map.end()) { map[interval.low()] = obj; };
            if (it != intervals.end()) { intervals.insert(interval); intervals.deoverlap(); };
            return this;
        };

        // 
        decltype(auto) erase( N const& address = 0ull) {
            auto it = intervals.overlap_find({ address, address });
            if (it != intervals.end()) {
                auto mt = map.find(it->interval().low());
                map.erase(mt);
                intervals.erase(it);
            };
            return this;
        };

        // 
        decltype(auto) find( N const& address = 0ull) {
            auto it = intervals.overlap_find({ address, address });
            return it != intervals.end() ? opt_ref<T>(map.at(it->interval().low())) : optional_ref<T>{};
        };

        // 
        decltype(auto) find( N const& address = 0ull) const {
            auto& intervals = const_cast<lib_interval_tree::interval_tree_t<N>&>(this->intervals);
            auto it = intervals.overlap_find({ address, address });
            return it != intervals.end() ? opt_cref<T>(map.at(it->interval().low())) : optional_ref<const T>{};
        };
    };
#endif

    // boolean 32-bit capable for C++
    class bool32_t { // TODO: support operators
    protected: union {
        uint32_t b_ : 1; bool bb = false;
    };
    public: friend bool32_t;
        constexpr bool32_t(): b_(0u) {};
        inline bool32_t(bool const&a=false): b_(a?1u:0u) {};
        inline bool32_t(uint32_t const&a): b_(a&1u) {}; // make bitmasked
        inline bool32_t(bool32_t const&a): b_(a) {};

        // type conversion operators
        inline operator bool() const {return bool(b_&1u);};
        inline operator uint32_t() const {return (b_&1u);};

        // 
        inline decltype(auto) operator=(const bool&a){b_=(a?1u:0u);return *this;};
        inline decltype(auto) operator=(const uint32_t&a){b_=a&1u;return *this;};
        inline decltype(auto) operator=(const bool32_t&a){b_=a;return *this;};
    };




};
