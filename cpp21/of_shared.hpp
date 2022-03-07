#pragma once 

// 
#include "./core.hpp"

// 
namespace cpp21 {

  // for vulkan structs with shared_ptr
  template<class T = void_t, template<class Ts = T> class W = wrap_shared_ptr, template<class Ts = T> class St = std::shared_ptr, template<class Ts = St<T>> class Vc = std::vector>
  class vector_of_shared {
  protected:
    using Sv = Vc<St<T>>;
    // we use local pointer memory
    Sv stack = {};

  public:
    // 
    inline vector_of_shared(Sv const& stack = {}) : stack(stack) {};

    // 
    inline decltype(auto) push(auto const& data = St<T>{}) {
      decltype(auto) last = stack.size();
      if constexpr (is_shared_ptr<std::decay_t<decltype(data)>>::value) {
        stack.push_back(std::reinterpret_pointer_cast<T>(data));
      }
      else {
        stack.push_back(std::reinterpret_pointer_cast<T>(copy_as_shared(data)));
      };
      return last;
    };

    // 
    //template<class Ts = T>
    //inline decltype(auto) push(St<Ts> const& data = St<T>{}) {
      //auto last = stack.size();
      //stack.push_back(std::reinterpret_pointer_cast<T>(data));
      //return last;
    //};

    // 
    template<class Ts = T>
    inline decltype(auto) get(uintptr_t const& index = 0u) {
      return W<Ts>(std::reinterpret_pointer_cast<Ts>(stack.at(index)));
      //return std::reinterpret_pointer_cast<Ts>(stack.at(index));
    };

    // 
    template<class Ts = T>
    inline decltype(auto) get(uintptr_t const& index = 0u) const {
      return W<Ts>(std::reinterpret_pointer_cast<Ts>(stack.at(index)));
      //return std::reinterpret_pointer_cast<Ts>(stack.at(index));
    };

    //
    template<class Ts = T> inline decltype(auto) at(uintptr_t const& index = 0u) { return this->get<Ts>(index); };
    template<class Ts = T> inline decltype(auto) at(uintptr_t const& index = 0u) const { return this->get<Ts>(index); };

    //
    template<class Ts = T>
    //inline decltype(auto) push_get(auto const& data = {}) { // don't prefer...
    inline decltype(auto) push_get(Ts const& data = {}) {
      return this->get<Ts>(this->push(data));
    };

    //
    inline operator Sv& () { return stack; };
    inline operator Sv const& () const { return stack; };

    //
    inline operator decltype(auto) (){ return this->data(); };
    inline operator decltype(auto) () const{ return this->data(); };

    //
    template<class Ts = T> inline decltype(auto) operator[](uintptr_t const& index) { return this->get<Ts>(index); };
    template<class Ts = T> inline decltype(auto) operator[](uintptr_t const& index) const { return this->get<Ts>(index); };

    //
    //inline decltype(auto) operator[](uintptr_t const& index) { return this->get<T>(index); };
    //inline decltype(auto) operator[](uintptr_t const& index) const { return this->get<T>(index); };

    //
    inline decltype(auto) clear() { stack.clear(); stack.resize(0u); };
    inline decltype(auto) size() const { return stack.size(); };
    inline decltype(auto) data() { return stack.data(); };
    inline decltype(auto) data() const { return stack.data(); };

    // 
    //template<class Ts = T> inline decltype(auto) back() { return std::reinterpret_pointer_cast<Ts>(stack.back()); };
    //template<class Ts = T> inline decltype(auto) back() const { return std::reinterpret_pointer_cast<Ts>(stack.back()); };
    template<class Ts = T> inline decltype(auto) back() { return W<Ts>(std::reinterpret_pointer_cast<Ts>(stack.back())); };
    template<class Ts = T> inline decltype(auto) back() const { return W<Ts>(std::reinterpret_pointer_cast<Ts>(stack.back())); };

    // 
    inline decltype(auto) push_back(auto const& data) const { return this->push(data); };

    //
    template<class Ts = T> inline decltype(auto) push_back(St<Ts> const& data = St<T>{}) { return this->push(data); };

    //
    inline decltype(auto) operator->() { return &stack; };
    inline decltype(auto) operator->() const { return &stack; };

    //
    inline decltype(auto) operator*() { return stack; };
    inline decltype(auto) operator*() const { return stack; };
  };



  // 
  //template<class T = void_t, template<class Ts = T> class W = uni_ptr, template<class Ts = std::shared_ptr<T>> class Vc = std::vector>
  //using vector_of_shared = vector_of_shared<T, W, std::shared_ptr, Vc<std::shared_ptr<T>>;

  // 
  //template<class T = void_t, template<class Ts = std::shared_ptr<T>> class Vc = std::vector>
  //using vector_of_shared = vector_of_shared < T, uni_ptr, std::shared_ptr, Vc<std::shared_ptr<T>>;

  // 
  //template<class T = void_t, template<class Ts = T> class St = std::shared_ptr, template<class Ts = St<T>> class Vc = std::vector>
  //using vector_of_shared = vector_of_shared < T, uni_ptr, std::shared_ptr, Vc<St<T>>;


  // for advanced vulkan structs with shared_ptr
  template<class K = uintptr_t, class T = void_t, template<class Ts = T> class W = wrap_shared_ptr, template<class Ts = T> class St = std::shared_ptr, template<class Ks = K, class Ts = St<T>> class Mc = std::unordered_map>
  class map_of_shared {
  protected:
    // we use local pointer memory
    Mc<K, St<T>> map = {};

  public:
    inline map_of_shared(Mc<K, St<T>> const& map = {}) : map(map) {};

    // 
    inline decltype(auto) set(K const& key, auto const& data = {}) {
      using Ts = std::decay_t<decltype(data)>;
      if constexpr (is_shared_ptr<Ts>::value) {
        using Tm = std::decay_t<decltype(*data)>;
        return W<Tm>(std::reinterpret_pointer_cast<Tm>(map[key] = std::reinterpret_pointer_cast<T>(data)));
        //return std::reinterpret_pointer_cast<Tm>(map[key] = std::reinterpret_pointer_cast<T>(data));
      }
      else {
        return W<Ts>(std::reinterpret_pointer_cast<Ts>(map[key] = std::reinterpret_pointer_cast<T>(copy_as_shared<Ts>(data))));
        //return std::reinterpret_pointer_cast<Ts>(map[key] = std::reinterpret_pointer_cast<T>(copy_as_shared(data)));
      };
      //return uni_ptr<Ts>{};
    };

    // 
    template<class Ts = T>
    inline decltype(auto) get(K const& key) {
      return W<Ts>(std::reinterpret_pointer_cast<Ts>(map.at(key)));
      //return std::reinterpret_pointer_cast<Ts>(map.at(key));
    };

    // 
    template<class Ts = T>
    inline decltype(auto) get(K const& key) const {
      return W<Ts>(std::reinterpret_pointer_cast<Ts>(map.at(key)));
      //return std::reinterpret_pointer_cast<Ts>(map.at(key));
    };

    //
    //template<class Ts = T> inline decltype(auto) operator[](K const& key) { return W<Ts>(std::reinterpret_pointer_cast<Ts>(map[key])); };
    //template<class Ts = T> inline decltype(auto) operator[](K const& key) const { return W<Ts>(std::reinterpret_pointer_cast<Ts>(map[key])); };

    //
    template<class Ts = T> inline decltype(auto) at(K const& index = 0u) { return this->get<Ts>(index); };
    template<class Ts = T> inline decltype(auto) at(K const& index = 0u) const { return this->get<Ts>(index); };

    //
    inline decltype(auto) operator->() { return &map; };
    inline decltype(auto) operator->() const { return &map; };

    //
    inline decltype(auto) operator*() { return map; };
    inline decltype(auto) operator*() const { return map; };
  };


};
