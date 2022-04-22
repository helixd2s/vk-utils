#pragma once 

// 
#ifdef __cplusplus
#include "./core.hpp"
#include "./shared_of.hpp"

// 
namespace cpp21 {

  /*
    // Official version of vector from descriptors of Ramen...
    // This class reusing free cells, instead erase or delete...
  */

  // 
  template<
    class T = void_t, 
    template<class Ts = T> class Vt = std::vector, 
    template<class Ts = T, class Vs = Vt<Ts>> class SVt = shared_vector
  > class bucket_;
  template<
    class T, 
    template<class Ts = T> class Vt = std::vector, 
    template<class Ts = T, class Vs = Vt<Ts>> class SVt = shared_vector
  > using bucket = bucket_<decay_t<T>, Vt, SVt>;

  // 
  template<
    class T, 
    template<class Ts> class Vt, 
    template<class Ts, class Vs> class SVt
  > class bucket_ {
  protected:
    SVt<T, Vt> used = {};
    SVt<uintptr_t, Vt> free = {};

  public:
    // 
    inline bucket_(bucket_<T, Vt, SVt> const& buck = {}) : used(buck.used), free(buck.free) {

    };

    // 
    inline bucket_(Vt<T> const& used = {}, Vt<uintptr_t> const& free = {}) : used(used), free(free) {

    };

    // 
    inline bucket_(SVt<T, Vt> const& used = {}, SVt<uintptr_t, Vt> const& free = {}) : used(used), free(free) {

    };

    // 
    inline decltype(auto) add(T const& e) {
      uintptr_t index = 0u;
      if (free->size() > 0) {
        used[index = free->back()] = e;
        free->pop_back();
      }
      else {
        index = used->size();
        used->push_back(e);
      };
      return index;
    };

    // 
    inline decltype(auto) removeByIndex(uintptr_t const& idx) {
      decltype(auto) last = used->size() - 1;
      if (idx <= last && last >= 0 && last != -1 && last < 0xFFFFFFFFFFFFFFFFull) {
        used[idx] = T{};
        if (idx == last) { used->resize(last); } else { free->push_back(idx); };
      };
      return *this;
    };

    //
    inline decltype(auto) removeByValue(T const& element = {}) {
      //decltype(auto) found = this->used.find();
      //return this->removeByIndex((found == this->used.end()) ? std::distance(used.begin(), found) : this->used.size());
      return this->removeByIndex(std::distance(used.begin(), this->used.find()));
    };

    //
    inline decltype(auto) remove(uintptr_t const& idx) {
      return this->removeByIndex(idx);
    };

    //
    template<static_not<is_vector<T>>>
    inline decltype(auto) operator=(T const& elem) { this->used = std::make_shared<Vt<T>>(&elem, &elem + 1u); this->free = Vt<uintptr_t>{}; return *this; };
    inline decltype(auto) operator=(Vt<T> const& vect) { this->used = std::make_shared<Vt<T>>(vect); this->free = Vt<uintptr_t>{}; return *this; };
    inline decltype(auto) operator=(SVt<T, Vt> const& vect) { this->used = vect; this->free = Vt<uintptr_t>{}; return *this; };
    inline decltype(auto) operator=(bucket_<T, Vt, SVt> const& vect) { this->used = vect.used; this->free = vect.free; return *this; };

    //
    inline operator SVt<T, Vt>& () { return used; };
    inline operator SVt<T, Vt> const& () const { return used; };

    //
    inline operator Vt<T>& () { return *used; };
    inline operator Vt<T> const& () const { return *used; };

    //
    inline decltype(auto) operator[](uintptr_t const& index) { return used->at(index); };
    inline decltype(auto) operator[](uintptr_t const& index) const { return used->at(index); };

    //
    inline decltype(auto) clear() { used->clear(); free->clear(); used->resize(0u); free->resize(0u); };
    inline decltype(auto) size() { return used->size(); };
    inline decltype(auto) data() { return used->data(); };
    inline decltype(auto) data() const { return used->data(); };

    //
    inline operator T* () { return used->data(); };
    inline operator T const* () const { return used->data(); };

    //
    inline decltype(auto) operator->() { return used.get(); };
    inline decltype(auto) operator->() const { return used.get(); };

    //
    inline decltype(auto) operator*() { return *used; };
    inline decltype(auto) operator*() const { return *used; };
  };

};

#endif
