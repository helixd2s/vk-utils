#pragma once 

// 
#ifdef __cplusplus
#include "./core.hpp"

// 
namespace cpp21 {

  // 
  template<
    class T = void_t,
    template<class Ts = T> class Sh = std::shared_ptr,
    template<class Ss = Sh<T>> class Sv = std::vector
  > class vector_of_shared_;
  template<
    class T = void_t,
    template<class Ts = T> class Sh = std::shared_ptr,
    template<class Ss = Sh<T>> class Sv = std::vector
  > using vector_of_shared = vector_of_shared_<decay_t<T>, Sh, Sv>;

  // for vulkan structs with shared_ptr
  template<
    class T,
    template<class Ts> class Sh,
    template<class Ss> class Sv
  >
  class vector_of_shared_ {
  protected:
    // we use local pointer memory
    Sv<Sh<T>> stack = {};

  public:
    // 
    inline vector_of_shared_(Sv<Sh<T>> const& stack = Sv<Sh<T>>{}) : stack(stack) {};
    inline vector_of_shared_(vector_of_shared<T> const& stack) : stack(stack) {};

    //
    inline operator bool() const { return !!stack; };
    inline decltype(auto) operator=(Sv<Sh<T>> const& stack) { this->stack = stack; return *this; };
    inline decltype(auto) operator=(vector_of_shared<T> const& stack) { this->stack = stack; return *this; };

    // 
    inline decltype(auto) push(auto const& data = Sh<T>{}) {
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
    inline operator Sv<Sh<T>>& () { return stack; };
    inline operator Sv<Sh<T>> const& () const { return stack; };

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
    template<class Ts = T> inline decltype(auto) push_back(Sh<Ts> const& data = Sh<T>{}) { return this->push(data); };

    //
    inline decltype(auto) operator->() { return &stack; };
    inline decltype(auto) operator->() const { return &stack; };

    //
    inline decltype(auto) operator*() { return stack; };
    inline decltype(auto) operator*() const { return stack; };
  };

  // 
  template<
    class K = uintptr_t, 
    class T = void_t,
    template<class Ts = T> class Sh = std::shared_ptr,
    template<class Ks = K, class Ss = Sh<T>> class Um = std::unordered_map
  > class map_of_shared_;

  // 
  template<
    class K = uintptr_t, 
    class T = void_t,
    template<class Ts = T> class Sh = std::shared_ptr,
    template<class Ks = K, class Ss = Sh<T>> class Um = std::unordered_map
  > using map_of_shared = map_of_shared_<decay_t<K>, decay_t<T>, Sh, Um>;

  // for advanced vulkan structs with shared_ptr
  template<
    class K, 
    class T,
    template<class Ts> class Sh,
    template<class Ks, class Ss> class Um
  >
  class map_of_shared_ : public std::enable_shared_from_this<map_of_shared<K,T,Sh,Um>> {
  protected:
    // we use local pointer memory
    using Umt = Um<K, Sh<T>>;
    Umt map = Umt{};

  public:
    inline map_of_shared_(Umt const& map = {}) : map(map) {};
    //inline operator bool() const { return !!map; };

    // 
    template<class Ts = T>
    inline decltype(auto) set(K const& key, Sh<T> data = {}) {
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
