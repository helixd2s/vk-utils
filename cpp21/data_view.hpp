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
    P<T> ptr = nullptr;
    size_t byteSize = 0ull;

  public:
    // 
    inline data_view(std::vector<T> const& ptr, uintptr_t const& offset = 0ull) : byteSize(ptr.size() * sizeof(T)), ptr(P<T>(const_cast<T*>(shift(ptr.data(), offset)))) {};
    inline data_view(data_view<T> const& ptr, uintptr_t const& offset = 0ull) : byteSize(ptr.size() * sizeof(T)), ptr(P<T>(const_cast<T*>(shift(ptr.data().get(), offset)))) {};
    inline data_view(P<T> const& ptr, size_t const& size, uintptr_t const& offset = 0ull) : byteSize(size), ptr(P<T>(const_cast<T*>(shift(ptr.get(), offset)))) {};
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
    inline operator P<T>& () { return this->ptr; };
    inline operator P<T> const& () const { return this->ptr; };

    //
    inline size_t size() const { return tiled(this->byteSize, sizeof(T)); };
    inline decltype(auto) data() { return ptr; };
    inline decltype(auto) data() const { return ptr; };

    //
    template<class Ts = T> inline decltype(auto) operator =(std::vector<Ts> const& wrap) { this->ptr = P<T>((T*)wrap.data()); this->byteSize = wrap.size() * sizeof(Ts); return *this; };
    template<class Ts = T> inline decltype(auto) operator =(data_view<Ts> const& wrap) { this->ptr = P<T>((T*)wrap.get()); this->byteSize = wrap.size() * sizeof(Ts); return *this; };
    template<class Ts = T> inline decltype(auto) operator =(wrap_ptr<Ts> const& wrap) { this->ptr = P<T>((T*)wrap.get()); return *this; };

    // 
    inline decltype(auto) operator =(auto const* const& ptr) { this->ptr = reinterpret_cast<P<T> const&>(ptr); return *this; };
    inline decltype(auto) operator =(auto* const& ptr) { this->ptr = reinterpret_cast<P<T> const&>(ptr); return *this; };
    inline decltype(auto) operator =(auto const& ref) { this->ptr = reinterpret_cast<P<T> const&>(&ref); return *this; };
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
