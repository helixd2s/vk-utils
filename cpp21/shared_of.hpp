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
  template<class T = void_t, template<class Ts = T> class V = std::vector, template<class Ts = T> class Sh = std::shared_ptr, class wT = std::remove_cv_t<std::decay_t<std::remove_cv_t<T>>>>
  class shared_vector {
  protected:
    using St = shared_vector<wT, V, Sh>;
    using Sv = shared_vector_t<wT, V, Sh>;
    using Vt = V<wT>;
    Sv vect = {};

  public:
    // 
    inline shared_vector() : vect(std::make_shared<Vt>()) {};

    // 
    //inline shared_vector(St&& vect) : vect(vect.vect) { };
    inline shared_vector(St const& vect) : vect(vect.vect) {};

    //
    //inline shared_vector(Vt&& vect) : vect(std::make_shared<Vt>(vect)) { };
    inline shared_vector(Vt vect) : vect(std::make_shared<Vt>(vect)) { };

    //
    template<static_not<is_vector<wT>>>
    inline shared_vector(wT const& elem) : vect(std::make_shared<Vt>(&elem, &elem+1u)) {

    };

    //
    template<static_not<is_vector<wT>>>
    inline decltype(auto) operator=(wT const& elem) { this->vect = std::make_shared<Vt>(&elem, &elem + 1u); return *this; };
    inline decltype(auto) operator=(Vt const& vect) { this->vect = std::make_shared<Vt>(vect); return *this; };
    inline decltype(auto) operator=(St const& vect) { this->vect = vect.vect; return *this; };
    

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
    //inline decltype(auto) assign(St const& v) { vect = v; return *this; };
    //inline decltype(auto) assign(Vt const& v) { *vect = v; return *this; };

    // 
    //inline decltype(auto) operator=(St const& v) { return this->assign(v); };
    //inline decltype(auto) operator=(Vt const& v) { return this->assign(v); };

    //
    inline decltype(auto) ref() { return *vect; };
    inline decltype(auto) ref() const { return *vect; };

    //
    inline decltype(auto) value() { return *vect; };
    inline decltype(auto) value() const { return *vect; };

    //
    inline decltype(auto) get() { return vect.get(); };
    inline decltype(auto) get() const { return vect.get(); };
  };


};
