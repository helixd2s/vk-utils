#pragma once 

// 
#include "./core.hpp"
#include "./wrap_ptr.hpp"

// 
namespace cpp21 {
  /*

  */

  // 
  template<class T = void_t, template<class Ts = T> class P = wrap_ptr>
  class data_view {
  protected:
    //P<T> ptr = nullptr;
    //size_t count = 0ull;
    std::span<T> ptr = {};

  public:
    // 
    inline data_view(std::span<T> const& wrap) : ptr(wrap) { ; };
    inline data_view(std::vector<T> const& wrap, uintptr_t const& offset = 0ull) : ptr(std::span<T>{const_cast<T*>(shift(wrap.data(), offset)), wrap.size()}) {};
    inline data_view(data_view<T> const& wrap, uintptr_t const& offset = 0ull) : ptr(std::span<T>{const_cast<T*>(shift(wrap.data(), offset)), wrap.size()}) {};
    inline data_view(P<T> const& wrap, size_t const& size = 1ull, uintptr_t const& offset = 0ull) : ptr(std::span<T>{const_cast<T*>(shift(wrap.get(), offset)), size}) {};
    inline data_view() {};

    // check operator
    inline operator bool() const { return !!this->ptr; };

    // type conversion
    inline operator T& () { return *this->ptr; };
    inline operator T const& () const { return *this->ptr; };

    // type conversion
    inline operator T* () { return this->ptr; };
    inline operator T const* () const { return this->ptr; };

    // type conversion
    inline operator std::span<T>&() { return reinterpret_cast<std::span<T>&>(*this); };
    inline operator std::span<T> const&() const { return reinterpret_cast<std::span<T> const&>(*this); };

    // type conversion
    inline operator P<T>& () { return this->ptr; };
    inline operator P<T> const& () const { return this->ptr; };

    //
    inline size_t size() const { return ptr.size(); };
    inline decltype(auto) data() { return ptr.data(); };
    inline decltype(auto) data() const { return ptr.data(); };

    //
    template<class Ts = T> inline decltype(auto) operator =(std::span<Ts> const& wrap) { this->ptr = wrap; return *this; };
    template<class Ts = T> inline decltype(auto) operator =(std::vector<Ts> const& wrap) { this->ptr = std::span<T>{ (T*)wrap.data(), wrap.size() }; return *this; };
    template<class Ts = T> inline decltype(auto) operator =(data_view<Ts> const& wrap) { this->ptr = std::span<T>{ (T*)wrap.get(), wrap.size() }; return *this; };
    template<class Ts = T> inline decltype(auto) operator =(wrap_ptr<Ts> const& wrap) { this->ptr = std::span<T>{ (T*)wrap.get(), 1ull }; return *this; };

    // 
    inline decltype(auto) operator =(auto const* const& ptr) { this->ptr = std::span<T>{ reinterpret_cast<P<T> const&>(ptr), 1ull }; return *this; };
    inline decltype(auto) operator =(auto* const& ptr) { this->ptr = std::span<T>{ reinterpret_cast<P<T> const&>(ptr), 1ull }; return *this; };
    inline decltype(auto) operator =(auto const& ref) { this->ptr = std::span<T>{ reinterpret_cast<P<T> const&>(&ref), 1ull }; return *this; };
    //inline decltype(auto) operator =(T const& ref) { ptr = ref; return *this; };
    //inline decltype(auto) operator =(T const* ptx) { ptr = ptx; return *this; };

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
};
