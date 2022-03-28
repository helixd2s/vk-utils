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

  template<class T = void_t> class bucket {
  protected:
    cpp21::shared_vector<T> used = {};
    cpp21::shared_vector<uintptr_t> free = {};

  public:
    // 
    inline bucket(std::vector<T> used = {}, std::vector<uintptr_t> free = {}) : used(used), free(free) {

    };

    // 
    inline bucket(cpp21::shared_vector<T> used = {}, cpp21::shared_vector<uintptr_t> free = {}) : used(used), free(free) {

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
    inline operator cpp21::shared_vector<T>& () { return used; };
    inline operator cpp21::shared_vector<T> const& () const { return used; };

    //
    inline operator std::vector<T>& () { return *used; };
    inline operator std::vector<T> const& () const { return *used; };

    //
    inline decltype(auto) operator[](uintptr_t const& index) { return used->at(index); };
    inline decltype(auto) operator[](uintptr_t const& index) const { return used->at(index); };

    //
    inline decltype(auto) clear() { used->clear(); free->clear(); used.resize(0u); free->resize(0u); };
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
    inline decltype(auto) operator*() { return used.get(); };
    inline decltype(auto) operator*() const { return used.get(); };
  };

};

#endif
