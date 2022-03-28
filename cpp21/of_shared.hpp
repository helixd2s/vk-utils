#pragma once 

// 
#ifdef __cplusplus
#include "./core.hpp"

// 
namespace cpp21 {

  // for vulkan structs with shared_ptr
  template<class T = void_t>
  class vector_of_shared {
  protected:
    using Sv = std::vector<std::shared_ptr<T>>;
    // we use local pointer memory
    Sv stack = {};

  public:
    // 
    inline vector_of_shared(Sv const& stack = {}) : stack(stack) {};

    //
    inline operator bool() const { return !!stack; };
    inline decltype(auto) operator=(Sv const& stack) { this->stack = stack; return *this; };

    // 
    inline decltype(auto) push(auto const& data = std::shared_ptr<T>{}) {
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
    template<class Ts = T>
    inline decltype(auto) get(uintptr_t const& index = 0u) {
      return std::reinterpret_pointer_cast<Ts>(stack.at(index));
    };

    // 
    template<class Ts = T>
    inline decltype(auto) get(uintptr_t const& index = 0u) const {
      return std::reinterpret_pointer_cast<Ts>(stack.at(index));
    };

    //
    template<class Ts = T> inline decltype(auto) at(uintptr_t const& index = 0u) { return this->get<Ts>(index); };
    template<class Ts = T> inline decltype(auto) at(uintptr_t const& index = 0u) const { return this->get<Ts>(index); };

    //
    template<class Ts = T>
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
    inline decltype(auto) clear() { stack.clear(); stack.resize(0u); };
    inline decltype(auto) size() const { return stack.size(); };
    inline decltype(auto) data() { return stack.data(); };
    inline decltype(auto) data() const { return stack.data(); };

    // 
    template<class Ts = T> inline decltype(auto) back() { return W<Ts>(std::reinterpret_pointer_cast<Ts>(stack.back())); };
    template<class Ts = T> inline decltype(auto) back() const { return W<Ts>(std::reinterpret_pointer_cast<Ts>(stack.back())); };

    // 
    inline decltype(auto) push_back(auto const& data) const { return this->push(data); };

    //
    template<class Ts = T> inline decltype(auto) push_back(std::shared_ptr<Ts> const& data = std::shared_ptr<T>{}) { return this->push(data); };

    //
    inline decltype(auto) operator->() { return &stack; };
    inline decltype(auto) operator->() const { return &stack; };

    //
    inline decltype(auto) operator*() { return stack; };
    inline decltype(auto) operator*() const { return stack; };
  };

  // for advanced vulkan structs with shared_ptr
  template<class K = uintptr_t, class T = void_t>
  class map_of_shared : public std::enable_shared_from_this<map_of_shared<K,T>> {
  protected:
    // we use local pointer memory
    std::unordered_map<K, std::shared_ptr<T>> map = std::unordered_map<K, std::shared_ptr<T>>{};

  public:
    inline map_of_shared(std::unordered_map<K, std::shared_ptr<T>> const& map = {}) : map(map) {};
    //inline operator bool() const { return !!map; };

    // 
    template<class Ts = T>
    inline decltype(auto) set(K const& key, std::shared_ptr<Ts> data = {}) {
      map[key] = std::reinterpret_pointer_cast<T>(data);
      return wrap_shared_ptr<Ts>(data);
    };

    // 
    template<class Ts = T>
    inline decltype(auto) set(K const& key, Ts const& data = {}) {
      auto constructed = copy_as_shared<Ts>(data);
      map[key] = std::reinterpret_pointer_cast<T>(constructed);
      return wrap_shared_ptr<Ts>(constructed);
    };

    // 
    template<class Ts = T>
    inline decltype(auto) get(K const& key) {
      if (map.find(key) != map.end()) {
        return wrap_shared_ptr<Ts>(std::reinterpret_pointer_cast<Ts>(map.at(key)));
      };
      return wrap_shared_ptr<Ts>();
    };

    // 
    template<class Ts = T>
    inline decltype(auto) get(K const& key) const {
      if (map.find(key) != map.end()) {
        return wrap_ptr<Ts>(std::reinterpret_pointer_cast<Ts>(map.at(key)));
      };
      return wrap_ptr<Ts>();
    };

    //
    template<class Ts = T> inline decltype(auto) at(K const& index = 0u) { return this->get<Ts>(index); };
    template<class Ts = T> inline decltype(auto) at(K const& index = 0u) const { return this->get<Ts>(index); };

    //
    inline auto& ref() { return map; };
    inline auto const& ref() const { return map; };

    //
    inline decltype(auto) operator->() { return &map; };
    inline decltype(auto) operator->() const { return &map; };

    //
    inline auto& operator*() { return map; };
    inline auto const& operator*() const { return map; };
  };


};
#endif
