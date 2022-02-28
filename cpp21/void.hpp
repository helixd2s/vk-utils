#pragma once 

// 
#include "./core.hpp"

// 
namespace cpp21 {

  /*
    // What is "void_t"? This is specific type, replacement for C void for most case. Less conflicts, more compatible, more functional. But sometime C++ may make this type 1-byte sized.
    // Mostly used with pointers and with address spaces, and operates with it.
  */

  //using void_t = uint8_t;
#pragma pack(0)
  __declspec(align(0)) class void_t {
  public:
    //uint8_t : 0;

    // 
    inline void_t() {};

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
    template<class Ts = void_t> inline operator Ts& () { return *this->get<Ts>(); };
    template<class Ts = void_t> inline operator Ts const& () const { return *this->get<Ts>(); };

    // 
    template<class Ts = void_t> inline operator Ts* () { return this->get<Ts>(); };
    template<class Ts = void_t> inline operator Ts const* () const { return this->get<Ts>(); };

    // 
    template<class Ts = void_t> inline decltype(auto) get() { return reinterpret_cast<Ts*>(this); };
    template<class Ts = void_t> inline decltype(auto) get() const { return reinterpret_cast<Ts const*>(this); };

    // 
    inline operator bool() const { return false; };
  };

};
