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
#include <utility>
#include <unordered_map>

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
namespace cpp21 {

    // 
#define CPP21_FN_ALIAS(NAME, CALL) \
    template <typename... Args>\
    inline decltype(auto) NAME(Args&&... args) {\
      return CALL(args...);\
    };

    // make short alias
#ifdef VKU_ENABLE_TYPE_SAFE
    namespace ts {
        using namespace type_safe;
    };
#endif

    // aggregate cache
    inline auto* cache = new unsigned char[256u*256u];

    // 
    inline decltype(auto) aggregate(auto const& str){
        using T = std::decay_t<decltype(str)>;
        memcpy(cache, &str, sizeof(T));
        return reinterpret_cast<T&>(*cache);
    };

    // 
    inline decltype(auto) aggregate(auto const& str, auto& cache_pointer){
        using T = std::decay_t<decltype(str)>;
        memcpy(cache, &str, sizeof(T));
        return reinterpret_cast<T&>(*cache);
    };

    // 
    inline decltype(auto) unlock32(auto& cache) {
        return reinterpret_cast<uint32_t&>(cache);
    };

    // 
    inline decltype(auto) unlock32(auto const& cache) {
        return reinterpret_cast<uint32_t const&>(cache);
    };

    // 
    inline decltype(auto) zero32(auto& cache) {
        return (reinterpret_cast<uint32_t&>(cache) = 0u);
    };

    // 
    inline decltype(auto) unlock64(auto& cache) {
        return reinterpret_cast<uint64_t&>(cache);
    };

    // 
    inline decltype(auto) unlock64(auto const& cache) {
        return reinterpret_cast<uint64_t const&>(cache);
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
    template<class T = std::string>
    inline decltype(auto) toCString(std::vector<char const*>& cVect, std::vector<T> const& Vs = {}) {
        cVect.resize(Vs.size());
        std::transform(Vs.begin(), Vs.end(), cVect.begin(), [](std::string& str) { return str.c_str(); });
        return cVect;
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

    /* 
        // What is "void_t"? This is specific type, replacement for C void for most case. Less conflicts, more compatible, more functional. But sometime C++ may make this type 1-byte sized.
        // Mostly used with pointers and with address spaces, and operates with it.
    */

    //using void_t = uint8_t;
#pragma pack(0)
__declspec(align(0)) class void_t { public: 
        //uint8_t : 0;

        // 
        inline void_t(){};

        //
        inline decltype(auto) assign(auto const* obj) { using Ts = std::decay_t<decltype(obj)>; memcpy(this, obj, sizeof(Ts)); return reinterpret_cast<Ts&>(*this); };
        inline decltype(auto) assign(auto const& obj) { return this->assign(&obj); };

        //
        inline decltype(auto) operator=(auto const& obj) { return this->assign(obj); };
        inline decltype(auto) operator=(auto const* obj) { return this->assign(obj); };

        // 
        template<class Ts = void_t> inline decltype(auto) operator->() { return this->get<Ts>(); };
        template<class Ts = void_t> inline decltype(auto) operator->() const { return this->get<Ts>(); };

        // 
        template<class Ts = void_t> inline decltype(auto) operator *() { return *this->get<Ts>(); };
        template<class Ts = void_t> inline decltype(auto) operator *() const { return *this->get<Ts>(); };

        // 
        template<class Ts = void_t> inline operator Ts&() { return *this->get<Ts>(); };
        template<class Ts = void_t> inline operator Ts const&() const { return *this->get<Ts>(); };

        // 
        template<class Ts = void_t> inline operator Ts*&() { return this->get<Ts>(); };
        template<class Ts = void_t> inline operator Ts* const&() const { return this->get<Ts>(); };

        // 
        template<class Ts = void_t> inline decltype(auto) get() { return reinterpret_cast<Ts*&>(this); };
        template<class Ts = void_t> inline decltype(auto) get() const { return reinterpret_cast<Ts* const&>(this); };

        // 
        inline operator bool() const { return false; };
    };

    /* 
        // What is wrap_ptr? This is wrapper for C pointers, for avoid some conflicts and problems.
        // There is no specifical features, only pointers compatibility...
        // Size of is 8...
        // WARNING! Doesn't support "nullptr"! Needs workaround...
    */

    // if you needed just pointer, just cast `reinterpret_cast<T*&>(some_wrap_ptr)`, because contain only one pointer
    template<class T = void_t>
    class wrap_ptr {
    protected:
        T* ptr = nullptr;

    public:
        constexpr wrap_ptr(T* const&& pt) : ptr(pt) {  };
        inline wrap_ptr(std::optional<T> const& opt) { this->ptr = opt ? &opt.value() : nullptr; };
        //explicit wrap_ptr(wrap_ptr<T> const& wrap) : ptr(wrap.get()) {};
        //explicit wrap_ptr(T const* const& ptr ) : ptr(ptr) {};

        // 
        //wrap_ptr(T*& ptr) : ptr(ptr) {};
        inline wrap_ptr(T* const& ptr) : ptr(ptr) {};
        //wrap_ptr(T const*& ptr) : ptr(ptr) {};
        //wrap_ptr(T const* const& ptr) : ptr(ptr) {};
        inline wrap_ptr() {};

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
        inline decltype(auto) operator =(wrap_ptr<T> const& wrap) { this->wrap = wrap.get(); return *this; };
        inline decltype(auto) operator =(auto const* const& ptr) { this->ptr = reinterpret_cast<T const*>(ptr); return *this; };
        inline decltype(auto) operator =(auto* const& ptr) { this->ptr = reinterpret_cast<T*>(ptr); return *this; };
        inline decltype(auto) operator =(auto const& ref) { this->ptr = reinterpret_cast<T*>(&ref); return *this; };

        // 
        inline operator T& () { return *this->get(); };
        inline operator T const& () const { return *this->get(); };

        // 
        inline operator T*& () { return this->get(); };
        inline operator T const* const& () const { return this->get(); };

        // 
        inline operator bool() const { return !!ptr; };
    };





    /* 
        // What is optional ref pointer? This is our implementation optional_ref from type_safe, more functional.
    */

    // 
    template<class T = void_t, template<class Ts = T> class P = wrap_ptr>
    class optional_ref {
    protected:
        P<T> ptr = nullptr;

    public: 
        // 
#ifdef TYPE_SAFE_OPTIONAL_REF_HPP_INCLUDED
        inline optional_ref(ts::optional_ref<T> const& ref) : ptr(ref ? ref.value() : nullptr) {};
        inline optional_ref(ts::optional_ref<T>& ref) : ptr(ref ? ref.value() : nullptr) {};
#endif

        // 
        explicit inline optional_ref(optional_ref<T> const& ref) : ptr(ref ? ref.value() : nullptr) {};
        explicit inline optional_ref(optional_ref<T>& ref) : ptr(ref ? ref.value() : nullptr) {};

        // 
        inline optional_ref(T const& ref) : ptr(&const_cast<T&>(ref)) {};
        //inline optional_ref(T& ref) : ptr(&ref) {};
        inline optional_ref() {};

        // check operator
        inline operator bool() const { return !!this->ptr; };

        // type conversion
        inline operator T&() { return *this->ptr; };
        inline operator T const&() const { return *this->ptr; };

        // type conversion
        inline operator T*() { return this->ptr; };
        inline operator T const*() const { return this->ptr; };

        // type conversion
        inline operator P<T>&() { return this->ptr; };
        inline operator P<T> const&() const { return this->ptr; };

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
        inline decltype(auto) operator=(optional_ref<T> const& ref) { ptr = ref ? &ref.value() : nullptr; return *this; };
        inline decltype(auto) operator=(optional_ref<T>& ref) { ptr = ref ? &ref.value() : nullptr; return *this; };
        inline decltype(auto) operator=(wrap_ptr<T> const& ptx) { ptr = ptx; return *this; };
        //inline decltype(auto) operator=(P<T> const& ptx) { ptr = ptx; return *this; };
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
    template<class T = void_t, template<class Ts = T> class P = wrap_ptr>
    inline decltype(auto) opt_ref(T& ref) {
        //using T = std::decay_t<decltype(ref)>;
        return optional_ref<T, P>(ref);
    };

    // 
    template<class T = void_t, template<class Ts = T> class P = wrap_ptr>
    inline decltype(auto) opt_cref(T const& ref) {
        //using T = const std::decay_t<decltype(ref)>;
        return optional_ref<T, P>(ref);
    };



    /* 
        // What is intrusive self-copy pointer? I tell about it later...
    */

    //
    template<class T = void_t>
    void _free(void* ptr_) {
        if (ptr_) { delete reinterpret_cast<T*>(ptr_); };
    };

    //
    template<class Tt = void_t, class Tf = void_t>
    void _memcpy(void* _to, void const* _from, size_t _size) {
        (*reinterpret_cast<Tt*>(_to)) = (*reinterpret_cast<Tf const*>(_from));
    };

    //
    template<class T>
    void* _malloc(size_t _size) {
        // TODO: correct division
        return (new T[_size/sizeof(T)]);
    };

    //
    template<class E = void_t, template<class Es = void_t> class Vw = wrap_ptr>
    class self_copy_intrusive;

    // 
    template<class T = void_t, template<class Es = void_t> class Iw = self_copy_intrusive>
    class self_copy_intrusive_t;

    // 
    template<class T = void_t, template<class Ts = T> class I = self_copy_intrusive_t>
    class self_copy_ptr;

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

        inline self_copy_intrusive(Vp ptr = nullptr, size_t const& size = 0ull) : ptr(ptr ? ptr : this->malloc(size)), size(size)
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
            return ::malloc(size);
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
                if (!this->ptr) { this->ptr = (void_t*)this->malloc(this->size = intrusive.size); }; 
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
            _memcpy<T,T>(_to,_from,_size);
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
        inline operator T&() { return this->ref(); };
        inline operator T const&() const { return this->ref(); };

        //
        inline operator T*() { return this->get(); };
        inline operator T const*() const { return this->get(); };

        //
        inline operator I&() { return dynamic_cast<I&>(*this); };
        inline operator I const&() const { return dynamic_cast<I const&>(*this); };
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
        inline self_copy_ptr(T* ptr = nullptr) : intrusive(new I( ptr )) { };
        inline self_copy_ptr(T& ptr) : intrusive(new I( &ptr )) { };

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
        inline operator T&() { return this->ref(); };
        inline operator T const&() const { return this->ref(); };

        //
        inline operator T*() { return this->get(); };
        inline operator T const*() const { return this->get(); };

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
        public: using I = Iw<T>; using Ie = enable_self_copy_from_this<T,Iw>;

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

    // 
    //template<class T = void_t, template<class Ts = T> class I = self_copy_intrusive_t>
    //using shared_self_copy = std::shared_ptr<self_copy_intrusive_t<I<T>>>;

    //
    template<class T = void_t, template<class Ts = T> class I = std::vector, template<class Ts = T> class Sh = std::shared_ptr>
    using shared_vector_t = Sh<I<T>>;

    //
    template<class K = uintptr_t, class T = void_t, template<class Ks = K, class Ts = T> class I = std::unordered_map, template<class Ts = T> class Sh = std::shared_ptr>
    using shared_map_t = Sh<I<K,T>>;
    
    //
    template<class T = void_t, template<class Ts = T> class V = std::vector, template<class Ts = T> class Sh = std::shared_ptr>
    class shared_vector {
    protected: 
        using St = shared_vector<T,V,Sh>;
        using Sv = shared_vector_t<T,V,Sh>;
        using Vt = V<T>;
        Sv vect = {};

    public: 
        // 
        inline shared_vector(St const& vect) : vect(vect) {
            
        };

        //
        inline shared_vector(V<T> const& vect = {}) : vect(std::make_shared<V<T>>(vect.begin(), vect.end())) {
            
        };

        // 
        inline operator Vt*() { return vect.get(); };
        inline operator Vt const*() const { return vect.get(); };

        // 
        inline operator Vt&() { return *vect; };
        inline operator Vt const&() const { return *vect; };

        // 
        inline operator Sv&() { return vect; };
        inline operator Sv const&() const { return vect; };

        //
        inline decltype(auto) operator[](uintptr_t const& index) { return vect->at(index); };
        inline decltype(auto) operator[](uintptr_t const& index) const { return vect->at(index); };

        //
        inline decltype(auto) operator*() { return *vect; };
        inline decltype(auto) operator*() const { return *vect; };

        //
        inline decltype(auto) operator->() { return vect.get(); };
        inline decltype(auto) operator->() const { return vect.get(); };

        // 
        inline decltype(auto) assign(St const& v) { vect = v; return *this; };
        inline decltype(auto) assign(Vt const& v) { *vect = v; return *this; };

        // 
        inline decltype(auto) operator=(St const& v) { return this->assign(v); };
        inline decltype(auto) operator=(Vt const& v) { return this->assign(v); };

        //
        inline decltype(auto) get() { return vect.get(); };
        inline decltype(auto) get() const { return vect.get(); };
    };



    /*
        // Official version of vector from descriptors of Ramen...
        // This class reusing free cells, instead erase or delete...
    */

    template<class T = void_t, template<class Ts = T> class Vm = std::vector, template<class Ts = T, template<class Ty = Ts> class Va = Vm> class Vs = cpp21::shared_vector> class bucket {
    protected:
      Vs<T, Vm> used = {};
      Vs<uintptr_t, Vm> free = {};

    public:
      // 
      inline bucket(std::vector<T> used = {}, Vm<uintptr_t> free = {}) : used(used), free(free) {

      };

      // 
      inline bucket(Vs<T> used = {}, Vs<uintptr_t> free = {}) : used(used), free(free) {

      };

      // 
      inline decltype(auto) add(T const& e) {
        uintptr_t index = 0u;
        if (free.size() > 0) {
          used[index = free.back()] = e;
          free.pop_back();
        }
        else {
          index = free.size();
          used.push_back(e);
        };
        return index;
      };

      // 
      inline decltype(auto) removeByIndex(uintptr_t const& idx) {
        decltype(auto) last = used->size() - 1;
        if (idx <= last) {
          used[idx] = {};
          if (idx == last) { used.resize(last); }
          else { free.push_back(idx); };
        };
        return *this;
      };

      //
      inline decltype(auto) removeByValue(T const& element = {}) {
        //decltype(auto) found = this->used.find();
        //return this->removeByIndex((found == this->used.end()) ? std::distance(used.begin(), found) : this->used.size());
        return this->removeByIndex(std::distance(used.begin(), this->used.find()));
      };

      //
      inline decltype(auto) remove(uintptr_t const& idx) {
        return this->removeByIndex(idx);
      };

      //
      inline operator Vs<T>& () { return used; };
      inline operator Vs<T> const& () const { return used; };

      //
      inline operator Vm<T>& () { return *used; };
      inline operator Vm<T> const& () const { return *used; };

      //
      inline decltype(auto) operator[](uintptr_t const& index) { return used->at(index); };
      inline decltype(auto) operator[](uintptr_t const& index) const { return used->at(index); };

      //
      inline decltype(auto) clear() { used->clear(); free.clear(); used.resize(0u); free.resize(0u); };
      inline decltype(auto) size() { return used->size(); };
      inline decltype(auto) data() { return used->data(); };
      inline decltype(auto) data() const { return used->data(); };

      //
      inline operator T* () { return used->data(); };
      inline operator T const* () const { return used->data(); };

      //
      inline decltype(auto) operator->() { return used.get(); };
      inline decltype(auto) operator->() const { return used.get(); };

      //
      inline decltype(auto) operator*() { return used.get(); };
      inline decltype(auto) operator*() const { return used.get(); };
    };



    /* 
        // What is link? This is self-copy pointer, but without known size for deep operations
        // Usable for type-known classes, methods, structures...
        // Size of only 8 byte, not nested...
    */

    //
    class link_void;

    //
    template<class T = void_t>
    class link;

    // 
    class link_void {
    protected: 
        wrap_ptr<void_t> ptr = {};

    public: 
        // 
        inline decltype(auto) assign(void_t const* obj, size_t const& size) { if (!this->ptr) { this->ptr = (void_t*)malloc(size); }; memcpy(this->ptr, obj, size); return *this; };
        inline decltype(auto) assign(void_t const* obj) { std::cerr << "sorry, but we doesn't know assign size" << std::endl; return *this; };
        inline decltype(auto) operator=(void_t const* obj) { return this->assign(obj); };

        // 
        inline link_void() {};
        inline link_void(void_t const* ptr) { this->assign(ptr); };
        inline ~link_void() { free(ptr.get()); };

        //
        //decltype(auto) assign(auto const* obj) { using Ts = std::decay(decltype(obj))::type; return reinterpret_cast<link<Ts>&>(this->assign(reinterpret_cast<void_t const*>(obj), sizeof(Ts))); };
        inline decltype(auto) assign(auto const* obj) {
            using Ts = std::decay_t<decltype(obj)>;
            if (!this->ptr) { this->ptr = new Ts; };
            *reinterpret_cast<Ts*>(this->ptr) = *obj;
            return reinterpret_cast<link<Ts>&>(*this);
        };

        // type aliasing
        inline decltype(auto) assign(auto const& obj) { return this->assign(&obj); };

        //
        inline decltype(auto) operator=(auto const& obj) { return this->assign(obj); };
        inline decltype(auto) operator=(auto const* obj) { return this->assign(obj); };

        // to avoid ambiguous
        template<class Ts = void_t, class Ls = link<Ts>>
        inline decltype(auto) operator=(Ls const& obj) { return this->assign(obj.get<Ts>()); };

        // 
        template<class Ts = void_t> inline decltype(auto) operator->() { return this->get(); };
        template<class Ts = void_t> inline decltype(auto) operator->() const { return this->get(); };

        // 
        template<class Ts = void_t> inline decltype(auto) operator *() { return *this->get(); };
        template<class Ts = void_t> inline decltype(auto) operator *() const { return *this->get(); };

        // 
        template<class Ts = void_t> inline operator Ts&() { return *this->get(); };
        template<class Ts = void_t> inline operator Ts const&() const { return *this->get(); };

        // 
        template<class Ts = void_t> inline operator Ts*&() { return this->get(); };
        template<class Ts = void_t> inline operator Ts* const&() const { return this->get(); };

        // 
        template<class Ts = void_t> inline decltype(auto) get() { return reinterpret_cast<Ts*&>(this->ptr); };
        template<class Ts = void_t> inline decltype(auto) get() const { return reinterpret_cast<Ts* const&>(this->ptr); };
    };

    // 
    template<class T>
    class link : public link_void { 

    public: 
        inline link() : link_void() {};
        inline link(const void* const& ptr) : link_void(ptr) {};
        inline link(const T* const& obj) { this->assign(obj); };
        inline link(const link<T>& obj) { this->assign(obj.get()); };

        // 
        inline decltype(auto) operator->() { return this->get(); };
        inline decltype(auto) operator->() const { return this->get(); };

        // 
        inline decltype(auto) operator *() { return *this->get(); };
        inline decltype(auto) operator *() const { return *this->get(); };

        // 
        inline operator T&() { return *this->get(); };
        inline operator T const&() const { return *this->get(); };

        // 
        inline operator T*&() { return this->get(); };
        inline operator T* const&() const { return this->get(); };

        // 
        inline decltype(auto) get() { return reinterpret_cast<T*&>(this->ptr); };
        inline decltype(auto) get() const { return reinterpret_cast<T* const&>(this->ptr); };

        //
        inline decltype(auto) assign(auto const* obj) { return reinterpret_cast<link<std::decay_t<decltype(obj)>>>(link_void::assign(obj)); };
        inline decltype(auto) assign(auto const& obj) { return reinterpret_cast<link<std::decay_t<decltype(obj)>>>(link_void::assign(obj)); };

        //
        inline decltype(auto) operator=(auto const* obj) { return this->assign(obj); };
        inline decltype(auto) operator=(auto const& obj) { return this->assign(obj); };

        // to avoid ambiguous
        template<class Ts = T, class Ls = link<Ts>>
        inline decltype(auto) operator=(Ls const& obj) { return this->assign(obj.get()); };
    };

    //
    template<class Ts = void_t>
    inline decltype(auto) pointer(optional_ref<Ts> ref) {
        return wrap_ptr<Ts>(ref ? (&ref.value()) : nullptr);
    };

    //
    template<class Ts = void_t>
    inline decltype(auto) pointer(optional_ref<const Ts> ref) {
        return wrap_ptr<Ts>(ref ? (&ref.value()) : nullptr);
    };

    //
    template<class Ts = void_t>
    inline decltype(auto) pointer(std::optional<Ts>& ref) {
        return wrap_ptr<Ts>(ref ? (&ref.value()) : nullptr);
    };

    //
    template<class Ts = void_t>
    inline decltype(auto) pointer(std::optional<Ts> const& ref) {
        return wrap_ptr<Ts>(ref ? (&ref.value()) : nullptr);
    };

    //
    template<class Ts = void_t>
    inline decltype(auto) pointer(std::optional<const Ts> const& ref) {
        return wrap_ptr<Ts>(ref ? (&ref.value()) : nullptr);
    };

#ifdef TYPE_SAFE_OPTIONAL_REF_HPP_INCLUDED
    //
    template<class Ts = void_t>
    inline decltype(auto) pointer(ts::optional_ref<Ts> ref) {
        return wrap_ptr<Ts>(ref ? (&ref.value()) : nullptr);
    };

    //
    template<class Ts = void_t>
    inline decltype(auto) pointer(ts::optional_ref<const Ts> ref) {
        return wrap_ptr<Ts>(ref ? (&ref.value()) : nullptr);
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
        std::optional<T> storage = std::nullopt_t;
    
    public: // 
        inline uni_arg() {};
        inline uni_arg(T const& t) { storage = t; };
        inline uni_arg(T const* t) { if (t) { storage = *t; }; };
        inline uni_arg(uni_ptr<T> const& p) : storage(*p) {}; // UnUsual and Vain
        inline uni_arg(uni_arg<T> const& a) : storage(*a) {};

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

        inline decltype(auto) push_back( T const& e) {
            if (queue.size() >= count) { queue.pop_front(); };
            queue.push_back(e);
            return this;
        };

        inline decltype(auto) back() { return queue.back(); };
        inline decltype(auto) back() const { return queue.back(); };
        inline decltype(auto) size() const { return queue.size(); };
    };


    // for vulkan structs or descriptor set data bucket
    template<class U = uint8_t, class Vc = std::vector<U>>
    class memory_stack {
    protected:
        // we use local pointer memory
        Vc memory = {};

    public: 
        // 
        inline memory_stack(Vc const& memory = {}) : memory(memory) {
            
        };

        // 
        inline decltype(auto) push(auto const& data) {
            using S = std::decay_t<decltype(data)>;
            auto last = memory.size();
            memory.resize(last + sizeof(S));
            //memcpy(memory.data() + last, &data, sizeof(S));
            reinterpret_cast<S&>(memory.data()) = data;
            return last;
        };

        // 
        template<class S = U>
        inline decltype(auto) get(localptr_t const& address) {
            return uni_ptr(reinterpret_cast<S*>(memory.data() + address));
        };

        // 
        template<class S = U>
        inline decltype(auto) get(localptr_t const& address) const {
            return uni_ptr(reinterpret_cast<const S*>(memory.data() + address));
        };
    };


    // for vulkan structs with shared_ptr
    template<class T = void_t, template<class Ts = T> class W = uni_ptr, template<class Ts = T> class St = std::shared_ptr, template<class Ts = St<T>> class Vc = std::vector>
    class vector_of_shared {
    protected:
        using Sv = Vc<St<T>>;
        // we use local pointer memory
        Sv stack = {};

    public: 
        // 
        inline vector_of_shared(Sv const& stack = {}) : stack(stack) {};

        // 
        inline decltype(auto) push(auto const& data = St<T>{}) {
            auto last = stack.size();
            stack.push_back(std::reinterpret_pointer_cast<T>(copy_as_shared(data)));
            return last;
        };

        // 
        template<class Ts = T>
        inline decltype(auto) push(std::shared_ptr<Ts> const& data = St<T>{}) {
          auto last = stack.size();
          stack.push_back(std::reinterpret_pointer_cast<T>(data));
          return last;
        };

        // 
        template<class Ts = T>
        inline decltype(auto) get(uintptr_t const& index = 0u) {
            return W(std::reinterpret_pointer_cast<Ts>(stack.at(index)));
        };

        // 
        template<class Ts = T>
        inline decltype(auto) get(uintptr_t const& index = 0u) const {
            return W(std::reinterpret_pointer_cast<Ts>(stack.at(index)));
        };

        //
        template<class Ts = T>
        //inline decltype(auto) push_get(auto const& data = {}) { // don't prefer...
        inline decltype(auto) push_get(Ts const& data = {}) {
            return this->get<Ts>(this->push(data));
        };

        //
        inline operator Sv& () { return stack; };
        inline operator Sv const& () const { return stack; };

        //
        inline operator St<T>* () { return stack.data(); };
        inline operator St<T> const* () const { return stack.data(); };

        //
        template<class Ts = T> inline decltype(auto) operator[](uintptr_t const& index) { return this->get<Ts>(index); };
        template<class Ts = T> inline decltype(auto) operator[](uintptr_t const& index) const { return this->get<Ts>(index); };

        //
        //inline decltype(auto) operator[](uintptr_t const& index) { return this->get<T>(index); };
        //inline decltype(auto) operator[](uintptr_t const& index) const { return this->get<T>(index); };

        //
        inline decltype(auto) clear() { stack.clear(); stack.resize(0u); };
        inline decltype(auto) size() { return stack.size(); };
        inline decltype(auto) data() { return stack.data(); };
        inline decltype(auto) data() const { return stack.data(); };

        // 
        template<class Ts = T> inline decltype(auto) back() { return W(std::reinterpret_pointer_cast<Ts>(stack.back())); };
        template<class Ts = T> inline decltype(auto) back() const { return W(std::reinterpret_pointer_cast<Ts>(stack.back())); };

        //
        inline decltype(auto) push_back(auto const& data = St<T>{}) { return this->push(data); };
        inline decltype(auto) push_back(auto const& data = St<T>{}) const { return this->push(data); };

        //
        inline decltype(auto) operator->() { return &stack; };
        inline decltype(auto) operator->() const { return &stack; };

        //
        inline decltype(auto) operator*() { return stack; };
        inline decltype(auto) operator*() const { return stack; };
    };


    // for advanced vulkan structs with shared_ptr
    template<class K = uintptr_t, class V = void_t>
    class map_of_shared {
    protected:
        // we use local pointer memory
        std::unordered_map<K, std::shared_ptr<V>> map = {};

    public: 
        inline map_of_shared(std::unordered_map<K, std::shared_ptr<V>> const& map = {}) : map(map) {};

        // 
        inline decltype(auto) set(K const& key, auto const& data = {}) {
            using T = std::decay_t<decltype(data)>;
            return uni_ptr<T>(std::reinterpret_pointer_cast<T>(
                map[key] = std::reinterpret_pointer_cast<V>(copy_as_shared(data))
            ));
        };

        // 
        template<class T = V>
        inline decltype(auto) set(K const& key, std::shared_ptr<T> const& data = {}) {
            return uni_ptr<T>(std::reinterpret_pointer_cast<T>(
                (map[key] = std::reinterpret_pointer_cast<V>(data))
            ));
        };

        // 
        template<class T = V>
        inline decltype(auto) get(K const& key) {
            return uni_ptr(std::reinterpret_pointer_cast<T>(map.at(key)));
        };

        // 
        template<class T = V>
        inline decltype(auto) get(K const& key) const {
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
        inline decltype(auto) insert( lib_interval_tree::interval<N, lib_interval_tree::closed> const& interval, T const& obj) {
            auto it = intervals.overlap_find({ interval.low(), interval.low() });
            if (map.find(interval.low()) == map.end()) { map[interval.low()] = obj; };
            if (it != intervals.end()) { intervals.insert(interval); intervals.deoverlap(); };
            return this;
        };

        // 
        inline decltype(auto) erase( N const& address = 0ull) {
            auto it = intervals.overlap_find({ address, address });
            if (it != intervals.end()) {
                auto mt = map.find(it->interval().low());
                map.erase(mt);
                intervals.erase(it);
            };
            return this;
        };

        // 
        inline decltype(auto) find( N const& address = 0ull) {
            auto it = intervals.overlap_find({ address, address });
            return it != intervals.end() ? opt_ref<T>(map.at(it->interval().low())) : optional_ref<T>{};
        };

        // 
        inline decltype(auto) find( N const& address = 0ull) const {
            auto& intervals = const_cast<lib_interval_tree::interval_tree_t<N>&>(this->intervals);
            auto it = intervals.overlap_find({ address, address });
            return it != intervals.end() ? opt_cref<T>(map.at(it->interval().low())) : optional_ref<const T>{};
        };
    };
#endif

};
