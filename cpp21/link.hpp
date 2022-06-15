#pragma once 

// 
#ifdef __cplusplus
#include "./core.hpp"
#include "./wrap_ptr.hpp"

// 
namespace cpp21 {
    /*
        // What is link? This is self-copy pointer, but without known size for deep operations
        // Usable for type-known classes, methods, structures...
        // Size of only 8 byte, not nested...
    */

    // 
    //template<class N, class T> class link_void_;
    //template<class N, class T> using link_void = link_void_<decay_t<T>>;

    // 
    class link_void {
    protected:
        wrap_ptr<void_t> ptr = nullptr;

    public:
        // 
        inline decltype(auto) assign(void_t const* obj, size_t const& size) { if (!this->ptr) { this->ptr = (void_t*)calloc(size, 1ull); }; memcpy(this->ptr, obj, size); return *this; };
        inline decltype(auto) assign(void_t const* obj) { std::cerr << "sorry, but we doesn't know assign size" << std::endl; return *this; };
        inline decltype(auto) operator=(void_t const* obj) { return this->assign(obj); };

        // 
        inline link_void() {};
        inline link_void(void_t const* ptr) { this->assign(ptr); };
        inline ~link_void() { free(ptr.get()); };

        //
        //decltype(auto) assign(auto const* obj) { using Ts = std::decay(decltype(obj))::type; return reinterpret_cast<link<Ts>&>(this->assign(reinterpret_cast<void_t const*>(obj), sizeof(Ts))); };
        inline decltype(auto) assign(auto const* obj) {
            using Ts = std::decay_t<decltype(obj)>;
            if (!this->ptr) { this->ptr = new Ts; };
            *reinterpret_cast<Ts*>(this->ptr) = *obj;
            return reinterpret_cast<link<Ts>&>(*this);
        };

        // type aliasing
        inline decltype(auto) assign(auto const& obj) { return this->assign(&obj); };

        //
        inline decltype(auto) operator=(auto const& obj) { return this->assign(obj); };
        inline decltype(auto) operator=(auto const* obj) { return this->assign(obj); };

        // to avoid ambiguous
        template<class Ts = void_t, class Ls = link<Ts>>
        inline decltype(auto) operator=(Ls const& obj) { return this->assign(obj.get<Ts>()); };

        // 
        template<class Ts = void_t> inline decltype(auto) operator->() { return this->get(); };
        template<class Ts = void_t> inline decltype(auto) operator->() const { return this->get(); };

        // 
        template<class Ts = void_t> inline decltype(auto) operator *() { return *this->get(); };
        template<class Ts = void_t> inline decltype(auto) operator *() const { return *this->get(); };

        // 
        template<class Ts = void_t> inline operator Ts& () { return *this->get(); };
        template<class Ts = void_t> inline operator Ts const& () const { return *this->get(); };

        // 
        template<class Ts = void_t> inline operator Ts*& () { return this->get(); };
        template<class Ts = void_t> inline operator Ts* const& () const { return this->get(); };

        // 
        template<class Ts = void_t> inline decltype(auto) get() { return reinterpret_cast<Ts*&>(this->ptr); };
        template<class Ts = void_t> inline decltype(auto) get() const { return reinterpret_cast<Ts* const&>(this->ptr); };
    };

    //
    //template<class T> class link_;
    //template<class T> using link = link_<decay_t<T>>;

    // 
    template<class T>
    class link : public link_void {

    public:
        inline link() : link_void() {};
        inline link(const void* const& ptr) : link_void(ptr) {};
        inline link(const T* const& obj) { this->assign(obj); };
        inline link(const link<T>& obj) { this->assign(obj.get()); };

        // 
        inline decltype(auto) operator->() { return this->get(); };
        inline decltype(auto) operator->() const { return this->get(); };

        // 
        inline decltype(auto) operator *() { return *this->get(); };
        inline decltype(auto) operator *() const { return *this->get(); };

        // 
        inline operator T& () { return *this->get(); };
        inline operator T const& () const { return *this->get(); };

        // 
        inline operator T*& () { return this->get(); };
        inline operator T* const& () const { return this->get(); };

        // 
        inline decltype(auto) get() { return reinterpret_cast<T*&>(this->ptr); };
        inline decltype(auto) get() const { return reinterpret_cast<T* const&>(this->ptr); };

        //
        inline decltype(auto) assign(auto const* obj) { return reinterpret_cast<link<std::decay_t<decltype(obj)>>>(link_void::assign(obj)); };
        inline decltype(auto) assign(auto const& obj) { return reinterpret_cast<link<std::decay_t<decltype(obj)>>>(link_void::assign(obj)); };

        //
        inline decltype(auto) operator=(auto const* obj) { return this->assign(obj); };
        inline decltype(auto) operator=(auto const& obj) { return this->assign(obj); };

        // to avoid ambiguous
        template<class Ts = T, class Ls = link<Ts>>
        inline decltype(auto) operator=(Ls const& obj) { return this->assign(obj.get()); };
    };

};
#endif