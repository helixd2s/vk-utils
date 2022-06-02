#pragma once 

// 
#ifdef __cplusplus
#include "./core.hpp"

// 
namespace cpp21 {

  //
  template<
    class T = void_t, 
    template<class Ts = T> class Vt = std::vector,
    template<class Vs = Vt<T>> class Sh = std::shared_ptr
  > class shared_vector_;
  template<
    class T = void_t, 
    template<class Ts = T> class Vt = std::vector,
    template<class Vs = Vt<T>> class Sh = std::shared_ptr
  > using shared_vector = shared_vector_<decay_t<T>, Vt, Sh>;

  //
  template<
    class T, 
    template<class Ts> class Vt,
    template<class Vs> class Sh
  >
  class shared_vector_ {
  protected:
    using St = shared_vector<T>;
    using Sv = Sh<Vt<T>>;
    Sv vect = {};

  public:
    // 
    inline shared_vector_() : vect(std::make_shared<Vt<T>>()) {};
    inline shared_vector_(St const& vect) : vect(vect.vect) {};
    inline shared_vector_(Sv const& vect) : vect(vect) {};
    inline shared_vector_(Vt<T> const& vect) : vect(std::make_shared<Vt<T>>(vect)) { };

    //
    template<static_not<is_vector<T>>> inline shared_vector_(T const& elem, size_t const& count = 1u) : vect(std::make_shared<Vt>(&elem, &elem+count)) {};
    template<static_not<is_vector<T>>> inline shared_vector_(T const* elem, size_t const& count = 1u) : vect(std::make_shared<Vt>(elem, elem+count)) {};
    template<static_not<is_vector<T>>> inline decltype(auto) assign(T const& elem, size_t const& count = 1u) { this->vect = std::make_shared<Vt<T>>(&elem, &elem + count); return *this; };
    template<static_not<is_vector<T>>> inline decltype(auto) assign(T const* elem, size_t const& count = 1u) { this->vect = std::make_shared<Vt<T>>(elem, elem + count); return *this; };
    template<static_not<is_vector<T>>> inline decltype(auto) operator=(T const& elem) { return this->assign(elem, 1u); };
    template<static_not<is_vector<T>>> inline decltype(auto) operator=(T const* elem) { return this->assign(elem, 1u); };

    //
    inline decltype(auto) operator=(Vt<T> const& vect) { this->vect = std::make_shared<Vt<T>>(vect); return *this; };
    inline decltype(auto) operator=(St const& vect) { this->vect = vect.vect; return *this; };

    // 
    inline operator Vt<T>* () { return vect.get(); };
    inline operator Vt<T> const* () const { return vect.get(); };

    // 
    inline operator Vt<T>& () { return *vect; };
    inline operator Vt<T> const& () const { return *vect; };

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
