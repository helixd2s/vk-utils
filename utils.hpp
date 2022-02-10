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

    //using void_t = uint8_t;
    class void_t { public: 

        //
        template<class Ts = void_t>
        decltype(auto) assign(Ts const* obj) { memcpy(this, obj, sizeof(Ts)); return reinterpret_cast<Ts&>(*this); };

        //
        template<class Ts = void_t>
        decltype(auto) operator=(Ts const& obj) { return this->assign<Ts>(&obj); };

        // 
        template<class Ts = void_t> Ts*& operator->() { return this->get<Ts>(); };
        template<class Ts = void_t> Ts* const& operator->() const { return this->get<Ts>(); };

        // 
        template<class Ts = void_t> Ts& operator *() { return *this->get<Ts>(); };
        template<class Ts = void_t> Ts const& operator *() const { return *this->get<Ts>(); };

        // 
        template<class Ts = void_t> operator Ts&() { return *this->get<Ts>(); };
        template<class Ts = void_t> operator Ts const&() const { return *this->get<Ts>(); };

        // 
        template<class Ts = void_t> operator Ts*&() { return this->get<Ts>(); };
        template<class Ts = void_t> operator Ts* const&() const { return this->get<Ts>(); };

        // 
        template<class Ts = void_t> Ts*& get() { return reinterpret_cast<Ts*&>(this); };
        template<class Ts = void_t> Ts* const& get() const { return reinterpret_cast<Ts* const&>(this); };

        // 
        operator bool() const { return false; };
    };

    // 
#ifdef TYPE_SAFE_OPTIONAL_REF_HPP_INCLUDED
    using optional_ref = ts::optional_ref;
    using opt_ref = ts::opt_ref;
    using opt_cref = ts::opt_cref;
#else 
    // own implementation of optional ref
    template<class T = void_t>
    class optional_ref {
    private:
        T* ptr = nullptr;

        // 
        optional_ref(optional_ref<T> const& ref) : ptr(ref ? ref.value() : nullptr) {};
        optional_ref(optional_ref<T>& ref) : ptr(ref ? ref.value() : nullptr) {};
        optional_ref(T const& ref) : ptr(&ref) {};
        optional_ref(T& ref) : ptr(&ref) {};
        optional_ref() {};

        // 
        operator T&() { return *this->ptr; }
        operator T const&() const { return *this->ptr; }

        // assign ref
        decltype(auto) operator=(optional_ref<T> const& ref) { ptr = ref ? ref.value() : nullptr; return *this; };
        decltype(auto) operator=(optional_ref<T>& ref) { ptr = ref ? ref.value() : nullptr; return *this; };
        decltype(auto) operator=(T& ref) { ptr = &ref; return *this; };
        decltype(auto) operator=(T const& ref) { *ptr = ref; return *this; };

        // value alias
        T& value() { return *this->ptr; }
        T const& value() const { return *this->ptr; }

        // unwrap operator
        T& operator *() { return *this->ptr; };
        T const& operator *() const { return *this->ptr; };

        // accessor operator
        T* operator ->() { return this->ptr; }
        T const* operator ->() const { return this->ptr; }

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
#endif

    // 
    template<class T>
    class wrap_ptr {
    protected:
        T* ptr = nullptr;
    public:
        wrap_ptr(T* ptr = nullptr) { this->ptr = ptr; };

        T*& operator->() { return this->get(); };
        T* const& operator->() const { return this->get(); };

        T& operator *() { return *this->get(); };
        T const& operator *() const { return *this->get(); };

        operator T& () { return *this->get(); };
        operator const T& () const { return *this->get(); };

        operator T*& () { return this->get(); };
        operator T* const& () const { return this->get(); };

        T& value() { return *this->get(); };
        T const& value() const { return *this->get(); };

        T*& get() { return reinterpret_cast<T*&>(ptr); };
        T* const& get() const { return reinterpret_cast<T* const&>(ptr); };

        wrap_ptr<T>& operator =(T* const& ptr) { this->ptr = ptr; return *this; };
        wrap_ptr<T,W=wrap_ptr<T>>& operator =(W const& ptr) { this->ptr = ptr.get(); return *this; };

        operator bool() const { return !!ptr; };
    };

    //
    template<class T = void_t>
    class link;

    // 
    class link_void {
    protected: 
        void_t* ptr = nullptr;

    public: 
        link_void() {};
        link_void(void_t const* ptr) { this->assign(ptr); };
        ~link_void() { free(ptr); };

        // 
        decltype(auto) assign(void_t const* obj, size_t const& size) { memcpy(this->ptr = (void_t*)malloc(size), obj, size); return *this; };
        decltype(auto) assign(void_t const* obj) { std::cerr << "sorry, but we doesn't know assign size" << std::endl; return *this; };
        decltype(auto) operator=(void_t const* obj) { return this->assign(obj); };

        //
        template<class Ts = void_t>
        decltype(auto) assign(Ts const* obj) { return reinterpret_cast<link<Ts>&>(this->assign(reinterpret_cast<void_t const*>(obj), sizeof(Ts))); };

        //
        template<class Ts = void_t>
        decltype(auto) operator=(Ts const& obj) { return this->assign<Ts>(&obj); };

        // to avoid ambiguous
        template<class Ts = void_t, typename Ls = link<Ts>>
        decltype(auto) operator=(Ls const& obj) { return this->assign<Ts>(obj.get()); };

        // 
        template<class Ts = void_t> Ts*& operator->() { return this->get(); };
        template<class Ts = void_t> Ts* const& operator->() const { return this->get(); };

        // 
        template<class Ts = void_t> Ts& operator *() { return *this->get(); };
        template<class Ts = void_t> Ts const& operator *() const { return *this->get(); };

        // 
        template<class Ts = void_t> operator Ts&() { return *this->get(); };
        template<class Ts = void_t> operator Ts const&() const { return *this->get(); };

        // 
        template<class Ts = void_t> operator Ts*&() { return this->get(); };
        template<class Ts = void_t> operator Ts* const&() const { return this->get(); };

        // 
        template<class Ts = void_t> Ts*& get() { return reinterpret_cast<T*&>(this->ptr); };
        template<class Ts = void_t> Ts* const& get() const { return reinterpret_cast<T* const&>(this->ptr); };
    };

    // 
    template<class T = void_t>
    class link : public link_void { 

    public: 
        link() : link_void() {};
        link(const void* const& ptr) : link_void(ptr) {};
        link(const T* const& obj) { this->assign<T>(obj); };
        link(const link<T>& obj) { this->assign<T>(obj.get()); };

        // 
        T*& operator->() { return this->get(); };
        T* const& operator->() const { return this->get(); };

        // 
        T& operator *() { return *this->get(); };
        T const& operator *() const { return *this->get(); };

        // 
        operator T&() { return *this->get(); };
        operator T const&() const { return *this->get(); };

        // 
        operator T*&() { return this->get(); };
        operator T* const&() const { return this->get(); };

        // 
        T*& get() { return reinterpret_cast<T*&>(this->ptr); };
        T* const& get() const { return reinterpret_cast<T* const&>(this->ptr); };

        //
        template<class Ts = T>
        decltype(auto) assign(Ts const* obj) { return reinterpret_cast<link<Ts>&>(this->assign(reinterpret_cast<void_t const*>(obj), sizeof(Ts))); };

        //
        template<class Ts = T>
        decltype(auto) operator=(Ts const& obj) { return this->assign<Ts>(&obj); };

        // to avoid ambiguous
        template<class Ts = T, typename Ls = link<Ts>>
        decltype(auto) operator=(Ls const& obj) { return this->assign<Ts>(obj.get()); };
    };

//#ifdef TYPE_SAFE_OPTIONAL_REF_HPP_INCLUDED
    //
    template<class T = uintptr_t>
    inline decltype(auto) pointer(optional_ref<T> ref) {
        return ref ? (&ref.value()) : nullptr;
    };

    //
    template<class T = uintptr_t>
    inline decltype(auto) pointer(optional_ref<const T> ref) {
        return ref ? (&ref.value()) : nullptr;
    };
//#endif

    //
    template<class T = uintptr_t>
    inline decltype(auto) pointer(std::optional<T>& ref) {
        return ref ? (&ref.value()) : nullptr;
    };

    //
    template<class T = uintptr_t>
    inline decltype(auto) pointer(std::optional<T> const& ref) {
        return ref ? (&ref.value()) : nullptr;
    };

    //
    template<class T = uintptr_t>
    inline decltype(auto) pointer(std::optional<const T> const& ref) {
        return ref ? (&ref.value()) : nullptr;
    };

    // 
    inline void* shift(void* data, uintptr_t offset = 0ull) { return (void*)(reinterpret_cast<uintptr_t&>(data) + offset); };
    inline const void* shift(const void* data, uintptr_t offset = 0ull) { return (const void*)(reinterpret_cast<const uintptr_t&>(data) + offset); };

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
    template<class T>
    inline decltype(auto) shared_struct(T const& data) {
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
    class shared_store {
    protected:
        // we use local pointer memory
        std::vector<std::shared_ptr<V>> chain = {};

    public: 
        template<class T = V>
        decltype(auto) push(T const& data = {}) {
            auto last = chain.size();
            auto data = std::shared_ptr<V>((V*)malloc(sizeof(T)), free);
            memcpy(data.get(), &data, sizeof(T));
            chain->push_back(data);
            return last;
        };

        template<class T = V>
        decltype(auto) push(std::shared_ptr<T> const& data = {}) {
            auto last = chain.size();
            chain->push_back(std::reinterpret_pointer_cast<V>(data));
            return last;
        };

        decltype(auto) get(uintptr_t const& index = 0u) {
            return std::reinterpret_pointer_cast<T>(chain[index]);
        };

        decltype(auto) get(uintptr_t const& index = 0u) const {
            return std::reinterpret_pointer_cast<T>(chain[index]);
        };
    };

    // for advanced vulkan structs with shared_ptr
    template<class K = uintptr_t, class V = void_t>
    class shared_map {
    protected:
        // we use local pointer memory
        std::unordered_map<K, std::shared_ptr<V>> map = {};

    public: 
        template<class T = V>
        decltype(auto) set(K const& key, T const& data = {}) {
            auto last = chain.size();
            auto local = std::shared_ptr<V>((V*)malloc(sizeof(T)), free);
            memcpy(local.get(), &data, sizeof(T));
            map[key] = local;
            return std::reinterpret_pointer_cast<T>(local);
        };

        template<class T = V>
        decltype(auto) set(K const& key, std::shared_ptr<T> const& data = {}) {
            map[key] = std::reinterpret_pointer_cast<V>(data); 
            return *this;
        };

        decltype(auto) get(K const& key) {
            return std::reinterpret_pointer_cast<T>(map.at(key));
        };

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
        decltype(auto) insert( lib_interval_tree::interval<N, lib_interval_tree::closed> const& interval, T const& obj) {
            auto it = intervals.overlap_find({ interval.low(), interval.low() });
            if (map.find(interval.low()) == map.end()) { map[interval.low()] = obj; };
            if (it != intervals.end()) { intervals.insert(interval); intervals.deoverlap(); };
            return this;
        };

        decltype(auto) erase( N const& address = 0ull) {
            auto it = intervals.overlap_find({ address, address });
            if (it != intervals.end()) {
                auto mt = map.find(it->interval().low());
                map.erase(mt);
                intervals.erase(it);
            };
            return this;
        };

        decltype(auto) find( N const& address = 0ull) {
            auto it = intervals.overlap_find({ address, address });
            return it != intervals.end() ? opt_ref<T>(map.at(it->interval().low())) : optional_ref<T>{};
        };

        decltype(auto) find( N const& address = 0ull) const {
            auto& intervals = const_cast<lib_interval_tree::interval_tree_t<N>&>(this->intervals);
            auto it = intervals.overlap_find({ address, address });
            return it != intervals.end() ? opt_cref<T>(map.at(it->interval().low())) : optional_ref<const T>{};
        };
    };
#endif

    

};
