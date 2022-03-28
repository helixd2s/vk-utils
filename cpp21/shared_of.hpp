#pragma once 

// 
#ifdef __cplusplus
#include "./core.hpp"

// 
namespace cpp21 {

  //
  template<class T = void_t>
  class shared_vector {
  protected:
    using wT = std::remove_cv_t<std::decay_t<std::remove_cv_t<T>>>;
    using St = shared_vector<wT>;
    using Sv = shared_vector_t<wT>;
    using Vt = std::vector<wT>;
    Sv vect = {};

  public:
    // 
    inline shared_vector() : vect(std::make_shared<Vt>()) {};
    inline shared_vector(St const& vect) : vect(vect.vect) {};
    inline shared_vector(Sv const& vect) : vect(vect) {};
    inline shared_vector(Vt const& vect) : vect(std::make_shared<Vt>(vect)) { };

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
    inline operator Sv() { return vect; };
    inline operator Sv() const { return vect; };

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
#endif
