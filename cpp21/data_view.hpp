#pragma once 

// 
#ifdef __cplusplus
#include "./core.hpp"
#include "./wrap_ptr.hpp"

// 
namespace cpp21 {
  /*

  */

  // 
  template<
    class T = std::byte, 
    template<class Ts = T> class Sp = std::span
  > class data_view_;

  // 
  template<
    class T = std::byte, 
    template<class Ts = T> class Sp = std::span
  > using data_view = data_view_<decay_t<T>, Sp>;

  // 
  template<class T, template<class Ts> class Sp>
  class data_view_ {
  protected:
    //P<T> ptr = nullptr;
    //size_t count = 0ull;
    Sp<T> ptr = {};

  public:
    // 
    inline data_view_(Sp<T> const& wrap) : ptr(wrap) { ; };
    inline data_view_(std::vector<T> const& wrap, uintptr_t const& offset = 0ull) : ptr(Sp<T>{const_cast<T*>(shift(wrap.data(), offset)), wrap.size()}) {};
    inline data_view_(data_view<T> const& wrap, uintptr_t const& offset = 0ull) : ptr(Sp<T>{const_cast<T*>(shift(wrap.data(), offset)), wrap.size()}) {};
    inline data_view_(wrap_ptr<T> const& wrap, uintptr_t const& offset = 0ull, size_t const& size = sizeof(T)) : ptr(Sp<T>{const_cast<T*>(shift(wrap.get(), offset)), size}) {};
    inline data_view_() {};

    // check operator
    inline operator bool() const { return !!this->ptr.data() && this->ptr.size() > 0; };

    // type conversion
    inline operator T& () { return *this->ptr.data(); };
    inline operator T const& () const { return *this->ptr.data(); };

    // type conversion
    inline operator T* () { return this->ptr.data(); };
    inline operator T const* () const { return this->ptr.data(); };

    // type conversion
    inline operator Sp<T>&() { return reinterpret_cast<Sp<T>&>(*this); };
    inline operator Sp<T> const&() const { return reinterpret_cast<Sp<T> const&>(*this); };

    // type conversion
    inline operator wrap_ptr<T>& () { return this->ptr; };
    inline operator wrap_ptr<T> const& () const { return this->ptr; };

    //
    inline size_t size() const { return ptr.size(); };
    inline decltype(auto) data() { return ptr.data(); };
    inline decltype(auto) data() const { return ptr.data(); };

    //
    template<class Ts = T> inline decltype(auto) operator =(Sp<Ts> const& wrap) { this->ptr = wrap; return *this; };
    template<class Ts = T> inline decltype(auto) operator =(std::vector<Ts> const& wrap) { this->ptr = Sp{ (T*)wrap.data(), wrap.size() }; return *this; };
    template<class Ts = T> inline decltype(auto) operator =(data_view<Ts> const& wrap) { this->ptr = Sp{ (T*)wrap.get(), wrap.size() }; return *this; };
    template<class Ts = T> inline decltype(auto) operator =(wrap_ptr<Ts> const& wrap) { this->ptr = Sp{ (T*)wrap.get(), 1ull }; return *this; };

    // 
    inline decltype(auto) operator =(auto const* const& ptr) { this->ptr = Sp{ reinterpret_cast<wrap_ptr<T> const&>(ptr), 1ull }; return *this; };
    inline decltype(auto) operator =(auto* const& ptr) { this->ptr = Sp{ reinterpret_cast<wrap_ptr<T> const&>(ptr), 1ull }; return *this; };
    inline decltype(auto) operator =(auto const& ref) { this->ptr = Sp{ reinterpret_cast<wrap_ptr<T> const&>(&ref), 1ull }; return *this; };
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

  //
  template<class T = std::byte, class B = std::byte>
  inline data_view<B, std::span> bytedata(std::vector<T> const& V, uintptr_t const& offset = 0ull) { return data_view<B, std::span>(reinterpret_cast<B*>(V.data()), offset, V.size() * sizeof(T)); };

};

#endif