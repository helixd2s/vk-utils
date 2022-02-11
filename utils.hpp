#pragma once

// 
#include <glm/glm.hpp>
#include <memory>
#include <vector>
#include <map>
#include <future>
#include <iostream>
#include <optional>
#include <coroutine>
#include <tuple>

// 
#ifdef VKU_ENABLE_TYPE_SAFE
#include <type_safe/optional_ref.hpp>
#include <type_safe/reference.hpp>
#include <type_safe/integer.hpp>
#endif

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

    //using void_t = uint8_t;
    class void_t { public: 
        //uint8_t : 0;

        // 
        void_t(){};

        //
        decltype(auto) assign(auto const* obj) { using Ts = std::decay(decltype(obj))::type; memcpy(this, obj, sizeof(Ts)); return reinterpret_cast<Ts&>(*this); };
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
    template<class T = void_t, class E = void_t, class Vp = void_t*>
    class self_copy_intrusive_t;

    // 
    template<class T = void_t, class I = self_copy_intrusive_t<T>>
    class self_copy_ptr;

    // E is extension in before pointer
    template<class E = void_t, class Vp = void_t*> // when extension is needed
    class self_copy_intrusive {
    public: using E = E; using Vp = Vp;
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

        self_copy_intrusive(
            Vp ptr = nullptr, size_t const& size = 0ull
        ) : ptr(ptr ? ptr : this->malloc(size)), size(size),
        {};

        self_copy_intrusive(self_copy_intrusive const& intrusive) {
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
        decltype(auto) assign(self_copy_intrusive const& I) { if (!this->ptr) { this->ptr = (void_t*)this->malloc(this->size = I->size); }; this->memcpy(this->ptr, I->ptr, I->size); return *this; };
        decltype(auto) operator=(self_copy_intrusive const& I) { return this->assign(I); };
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
    template<class T = void_t, class E = void_t, class Vp = void_t*>
    class self_copy_intrusive_t : public self_copy_intrusive<E> {
    public: using T = T; using E = E; using Vp = Vp;
        // if changable
        self_copy_intrusive_t(T& ref, size_t const& size = sizeof(T)) : self_copy_intrusive(reinterpret_cast<Vp*>(&ref), size ? size : sizeof(T)) {  };
        self_copy_intrusive_t(T* ptr, size_t const& size = sizeof(T)) : self_copy_intrusive(reinterpret_cast<Vp*>(ptr), size ? size : sizeof(T)) {  };

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
        decltype(auto) assign(T const& ref) { self_copy_intrusive::assign(ref); return *this; };
        decltype(auto) assign(T const* ptr = nullptr) { self_copy_intrusive::assign(ptr); return *this; };

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
        operator self_copy_intrusive&() { return dynamic_cast<self_copy_intrusive&>(*this); };
        operator self_copy_intrusive const&() const { return dynamic_cast<self_copy_intrusive const&>(*this); };
    };

    // 
    template<class T = void_t, class E = void_t, class Vp = void_t*, class I = self_copy_intrusive_t<T,E,Vp>>
    class enable_self_copy_from_this : public I {
        public: using T = T; using E = E; using Vp = Vp; using I = I;

        // 
        enable_self_copy_from_this() : I(reinterpret_cast<T*>(this), sizeof(T)) {};

        // 
        decltype(auto) self_copy_from_this() {
            using Ts = std::decay(decltype(this))::type;
            reinterpret_cast<T*>(this->ptr) = reinterpret_cast<T*>(this); // for any case
            return self_copy_ptr<T>(dynamic_cast<I&>(*this));
        };

        // 
        decltype(auto) self_copy_from_this() const {
            using Ts = std::decay(decltype(this))::type;
            const_cast<T*>(reinterpret_cast<T const*>(this->ptr)) = const_cast<T*>(reinterpret_cast<T const*>(this)); // for any case
            return self_copy_ptr<const T>(dynamic_cast<I&>(*this));
        };
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
        self_copy_ptr(I* intrusive = nullptr) { this->intrusive = intrusive; };
        self_copy_ptr(I& intrusive)  { this->intrusive = &intrusive; };

        // 
        self_copy_ptr(T* ptr = nullptr) { this->ptr = new I( ptr ); };
        self_copy_ptr(T& ptr) { this->ptr = new I( &ptr ); };

        //
        ~self_copy_ptr() { delete this->intrusive; }

        //
        decltype(auto) get() { this->intrusive->get<T>(); };
        decltype(auto) get() const { this->intrusive->get<T>(); };

        //
        decltype(auto) ref() { this->intrusive->ref<T>(); };
        decltype(auto) ref() const { this->intrusive->ref<T>(); };

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
        link_void() {};
        link_void(void_t const* ptr) { this->assign(ptr); };
        ~link_void() { free(ptr); };

        // 
        decltype(auto) assign(void_t const* obj, size_t const& size) { if (!this->ptr) { this->ptr = (void_t*)malloc(size) }; memcpy(this->ptr, obj, size); return *this; };
        decltype(auto) assign(void_t const* obj) { std::cerr << "sorry, but we doesn't know assign size" << std::endl; return *this; };
        decltype(auto) operator=(void_t const* obj) { return this->assign(obj); };

        //
        //decltype(auto) assign(auto const* obj) { using Ts = std::decay(decltype(obj))::type; return reinterpret_cast<link<Ts>&>(this->assign(reinterpret_cast<void_t const*>(obj), sizeof(Ts))); };
        decltype(auto) assign(auto const* obj) {
            using Ts = std::decay(decltype(obj))::type;
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
    [[deprecated]]
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
        decltype(auto) assign(auto const* obj) { return reinterpret_cast<link<std::decay(decltype(obj))::type>>(link_void::assign(obj)); };
        decltype(auto) assign(auto const& obj) { return reinterpret_cast<link<std::decay(decltype(obj))::type>>(link_void::assign(obj)); };

        //
        decltype(auto) operator=(auto const* obj) { return this->assign(obj); };
        decltype(auto) operator=(auto const& obj) { return this->assign(obj); };

        // to avoid ambiguous
        template<class Ts = T, class Ls = link<Ts>>
        decltype(auto) operator=(Ls const& obj) { return this->assign(obj.get()); };
    };

//#ifdef TYPE_SAFE_OPTIONAL_REF_HPP_INCLUDED
    //
    inline decltype(auto) pointer(optional_ref<auto> ref) {
        return ref ? (&ref.value()) : nullptr;
    };

    //
    inline decltype(auto) pointer(optional_ref<const auto> ref) {
        return ref ? (&ref.value()) : nullptr;
    };
//#endif

    //
    inline decltype(auto) pointer(std::optional<auto>& ref) {
        return ref ? (&ref.value()) : nullptr;
    };

    //
    inline decltype(auto) pointer(std::optional<auto> const& ref) {
        return ref ? (&ref.value()) : nullptr;
    };

    //
    inline decltype(auto) pointer(std::optional<const auto> const& ref) {
        return ref ? (&ref.value()) : nullptr;
    };

    // 
    inline decltype(auto) shift(auto* data, uintptr_t offset = 0ull) { return reinterpret_cast<decltype(data)>(reinterpret_cast<uintptr_t&>(data) + offset); };
    inline decltype(auto) shift(const auto* data, uintptr_t offset = 0ull) { return reinterpret_cast<decltype(data)>(reinterpret_cast<const uintptr_t&>(data) + offset); };

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

    // only for construct vulkan structures
    //template<class T>
    inline decltype(auto) shared_struct(auto const& data) {
        using T = decltype(data);
        auto shared = std::shared_ptr<T>((T*)malloc(sizeof(T)), free);
        //memcpy(shared.get(), &data, sizeof(T));
        *shared = data; // what if structure can self-copy?
        return shared;
    };

    //
    using localptr_t = uintptr_t;

    // for vulkan structs or descriptor set data bucket
    class data_store {
    protected:
        // we use local pointer memory
        std::vector<uint8_t> memory = {};

    public: 
        // 
        data_store(std::vector<uint8_t> const& memory = {}) : memory(memory) {
            
        };

        // 
        template<class S>
        decltype(auto) push(S const& data) {
            auto last = memory.size();
            memory.resize(last + sizeof(S));
            memcpy(memory.data() + last, &data, sizeof(S));
            return last;
        };

        // 
        template<class S>
        decltype(auto) ptr(localptr_t const& address) const {
            return reinterpret_cast<S const*>(memory.data() + address);
        };

        // 
        template<class S>
        decltype(auto) ptr(localptr_t const& address) {
            return reinterpret_cast<S*>(memory.data() + address);
        };

        // 
        template<class S>
        decltype(auto) ref(localptr_t const& address) {
            return *this->ptr<S>(address);
        };

        // 
        template<class S>
        decltype(auto) const& ref(localptr_t const& address) const {
            return *this->ptr<S>(address);
        };
    };

    // for vulkan structs with shared_ptr
    template<class V = void_t>
    class vector_of_shared {
    protected:
        // we use local pointer memory
        std::vector<std::shared_ptr<V>> chain = {};

    public: 
        // 
        vector_of_shared(std::vector<std::shared_ptr<V>> const& chain) : chain(chain) {};

        // 
        decltype(auto) push(auto const& data = {}) {
            auto last = chain.size();
            auto data = std::shared_ptr<V>((V*)malloc(sizeof(T)), free);
            memcpy(data.get(), &data, sizeof(T));
            chain->push_back(data);
            return last;
        };

        // 
        decltype(auto) push(std::shared_ptr<auto> const& data = {}) {
            auto last = chain.size();
            chain->push_back(std::reinterpret_pointer_cast<V>(data));
            return last;
        };

        // 
        decltype(auto) get(uintptr_t const& index = 0u) {
            return std::reinterpret_pointer_cast<T>(chain[index]);
        };

        // 
        decltype(auto) get(uintptr_t const& index = 0u) const {
            return std::reinterpret_pointer_cast<T>(chain[index]);
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
            auto last = chain.size();
            auto local = std::shared_ptr<V>((V*)malloc(sizeof(T)), free);
            memcpy(local.get(), &data, sizeof(T));
            map[key] = local;
            return std::reinterpret_pointer_cast<T>(local);
        };

        // 
        decltype(auto) set(auto const& key, std::shared_ptr<auto> const& data = {}) {
            map[key] = std::reinterpret_pointer_cast<V>(data); return *this;
        };

        // 
        decltype(auto) get(K const& key) {
            return std::reinterpret_pointer_cast<T>(map.at(key));
        };

        // 
        decltype(auto) get(K const& key) const {
            return std::reinterpret_pointer_cast<T>(map.at(key));
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

    

};
