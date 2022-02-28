#pragma once 

// 
#include "./core.hpp"

// 
namespace cpp21 {

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

};