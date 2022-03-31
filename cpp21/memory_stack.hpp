#pragma once 

// 
#ifdef __cplusplus
#include "./core.hpp"

// 
namespace cpp21 {

  // 
  template<
    class U = uint8_t, 
    template<class Us = U> class Vc = std::vector
  > class memory_stack_;
  template<
    class U = uint8_t, 
    template<class Us = U> class Vc = std::vector
  > using memory_stack = memory_stack_<decay_t<U>, Vc>;

  // for vulkan structs or descriptor set data bucket
  template<
    class U, 
    template<class Us> class Vc
  >
  class memory_stack_ {
  protected:
    // we use local pointer memory
    Vc<U> memory = {};

  public:
    // 
    inline memory_stack_(Vc<U> const& memory = {}) : memory(memory) {

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
#endif