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

    //using void_t = uint8_t;
    class void_t { public: 

    };


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
        wrap_ptr<T>& operator =(const wrap_ptr<T>& ptr) { this->ptr = ptr.get(); return *this; };

        operator bool() const { return !!ptr; };
    };




    class link_base {
    protected: 
        void* ptr = nullptr;

    public: 
        link_base() {};
        link_base(const void* ptr) { this->assign(ptr); };
        ~link_base() { free(ptr); };

        virtual link_base& assign(const void* obj, const uintptr_t& size) { memcpy(this->ptr = malloc(size), obj, size); return *this; };
        virtual link_base& assign(const void* obj) { return *this; };
        virtual link_base& operator=(const void* obj) { return this->assign(obj); };
    };

    template<class T>
    class link : public link_base { 
    protected: 
        T* ptr = nullptr;

    public: 
        link() : link_base() {};
        link(const void* const& ptr) : link_base(ptr) {};
        link(const T* const& obj) { this->assign(obj); };
        link(const link<T>& obj) { this->assign(obj.get()); };

        T*& operator->() { return this->get(); };
        T* const& operator->() const { return this->get(); };

        T& operator *() { return *this->get(); };
        T const& operator *() const { return *this->get(); };

        operator T&() { return *this->get(); };
        operator T const&() const { return *this->get(); };

        operator T*&() { return this->get(); };
        operator T* const&() const { return this->get(); };

        T*& get() { return reinterpret_cast<T*&>(ptr); };
        T* const& get() const { return reinterpret_cast<T* const&>(ptr); };

        // initiate assign operators
        virtual link_base& assign(const void* obj) override { this->ptr = new T; *this->get() = *reinterpret_cast<const T*&>(obj); return *this; };

        link<T>& operator=(const T& obj) { this->assign(&obj); return *this; };
        link<T>& operator=(const link<T>& obj) { this->assign(obj.get()); return *this; };

    };


        //
    template<class T = uintptr_t>
    inline T* pointer(ts::optional_ref<T> ref) {
        return ref ? (&ref.value()) : nullptr;
    };

    //
    template<class T = uintptr_t>
    inline T const* pointer(ts::optional_ref<const T> ref) {
        return ref ? (&ref.value()) : nullptr;
    };
    
    //
    template<class T = uintptr_t>
    inline T* pointer(std::optional<T>& ref) {
        return ref ? (&ref.value()) : nullptr;
    };

    //
    template<class T = uintptr_t>
    inline T const* pointer(std::optional<T> const& ref) {
        return ref ? (&ref.value()) : nullptr;
    };

    //
    template<class T = uintptr_t>
    inline T const* pointer(std::optional<const T> const& ref) {
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

#ifdef VKU_ENABLE_INTERVAL
    // 
    template<class N, class T>
    class interval_map { public: 
        std::unordered_map<N, T> map = {};
        lib_interval_tree::interval_tree_t<N> intervals = {};

    public:
        interval_map* insert( lib_interval_tree::interval<N, lib_interval_tree::closed> const& interval, T const& obj) {
            auto it = intervals.overlap_find({ interval.low(), interval.low() });
            if (map.find(interval.low()) == map.end()) { map[interval.low()] = obj; };
            if (it != intervals.end()) { intervals.insert(interval); intervals.deoverlap(); };
            return this;
        };

        interval_map* erase( N const& address = 0ull) {
            auto it = intervals.overlap_find({ address, address });
            if (it != intervals.end()) {
                auto mt = map.find(it->interval().low());
                map.erase(mt);
                intervals.erase(it);
            };
            return this;
        };

        ts::optional_ref<T> find( N const& address = 0ull) {
            auto it = intervals.overlap_find({ address, address });
            return it != intervals.end() ? ts::opt_ref<T>(map.at(it->interval().low())) : ts::optional_ref<T>{};
        };

        ts::optional_ref<const T> find( N const& address = 0ull) const {
            auto& intervals = const_cast<lib_interval_tree::interval_tree_t<N>&>(this->intervals);
            auto it = intervals.overlap_find({ address, address });
            return it != intervals.end() ? ts::opt_cref<T>(map.at(it->interval().low())) : ts::optional_ref<const T>{};
        };
    };
#endif

    

};
