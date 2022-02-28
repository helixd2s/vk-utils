#pragma once 

// 
#include "./core.hpp"

// 
namespace cpp21 {

  // 
//template<class T = void_t, template<class Ts = T> class I = self_copy_intrusive_t>
//using shared_self_copy = std::shared_ptr<self_copy_intrusive_t<I<T>>>;

//
  template<class T = void_t, template<class Ts = T> class I = std::vector, template<class Ts = T> class Sh = std::shared_ptr>
  using shared_vector_t = Sh<I<T>>;

  //
  template<class K = uintptr_t, class T = void_t, template<class Ks = K, class Ts = T> class I = std::unordered_map, template<class Ts = T> class Sh = std::shared_ptr>
  using shared_map_t = Sh<I<K, T>>;

  //
  template<class T = void_t, template<class Ts = T> class V = std::vector, template<class Ts = T> class Sh = std::shared_ptr>
  class shared_vector {
  protected:
    using St = shared_vector<T, V, Sh>;
    using Sv = shared_vector_t<T, V, Sh>;
    using Vt = V<T>;
    Sv vect = {};

  public:
    // 
    inline shared_vector(St const& vect) : vect(vect) {

    };

    //
    inline shared_vector(V<T> const& vect = {}) : vect(std::make_shared<V<T>>(vect.begin(), vect.end())) {

    };

    // 
    inline operator Vt* () { return vect.get(); };
    inline operator Vt const* () const { return vect.get(); };

    // 
    inline operator Vt& () { return *vect; };
    inline operator Vt const& () const { return *vect; };

    // 
    inline operator Sv& () { return vect; };
    inline operator Sv const& () const { return vect; };

    //
    inline decltype(auto) operator[](uintptr_t const& index) { return vect->at(index); };
    inline decltype(auto) operator[](uintptr_t const& index) const { return vect->at(index); };

    //
    inline decltype(auto) operator*() { return *vect; };
    inline decltype(auto) operator*() const { return *vect; };

    //
    inline decltype(auto) operator->() { return vect.get(); };
    inline decltype(auto) operator->() const { return vect.get(); };

    // 
    inline decltype(auto) assign(St const& v) { vect = v; return *this; };
    inline decltype(auto) assign(Vt const& v) { *vect = v; return *this; };

    // 
    inline decltype(auto) operator=(St const& v) { return this->assign(v); };
    inline decltype(auto) operator=(Vt const& v) { return this->assign(v); };

    //
    inline decltype(auto) get() { return vect.get(); };
    inline decltype(auto) get() const { return vect.get(); };
  };


};
