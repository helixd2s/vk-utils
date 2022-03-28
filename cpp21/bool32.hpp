#pragma once 

// 
#ifdef __cplusplus
#include "./core.hpp"

// 
namespace cpp21 {

  // boolean 32-bit capable for C++
  class bool32_t { // TODO: support operators
  protected: union {
    uint32_t b_ : 1; bool bb = false;
  };
  public: friend bool32_t;
        constexpr bool32_t() : b_(0u) {};
        inline bool32_t(bool const& a = false) : b_(a ? 1u : 0u) {};
        inline bool32_t(uint32_t const& a) : b_(a & 1u) {}; // make bitmasked
        inline bool32_t(bool32_t const& a) : b_(a) {};

        // type conversion operators
        inline operator bool() const { return bool(b_ & 1u); };
        inline operator uint32_t() const { return (b_ & 1u); };

        // 
        inline decltype(auto) operator=(const bool& a) { b_ = (a ? 1u : 0u); return *this; };
        inline decltype(auto) operator=(const uint32_t& a) { b_ = a & 1u; return *this; };
        inline decltype(auto) operator=(const bool32_t& a) { b_ = a; return *this; };
  };

};
#endif