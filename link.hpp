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

};
