#pragma once 

// 
#ifdef __cplusplus
#include "./core.hpp"
#include "./wrap_ptr.hpp"

// 
namespace cpp21 {

    // 
    template<class T = void_t> class optional_ref_;
    template<class T = void_t> using optional_ref = optional_ref_<decay_t<T>>;

    // 
    template<class T>
    class optional_ref_ {
    protected:
        T* ptr = nullptr;
        bool reallocated = false;

    public:

        // 
        inline ~optional_ref_() { if (reallocated) { delete ptr; }; };
        inline optional_ref_(std::optional<T>& ref) : ptr(const_cast<T*>(ref ? &(ref.value()) : nullptr)) {};
        inline optional_ref_(optional_ref<T>& ref) : ptr(const_cast<T*>(ref ? &(ref.value()) : nullptr)) {};
        inline optional_ref_(T& ref) : ptr(&ref) {};
        inline optional_ref_(T&& ref) : ptr(new T(ref)), reallocated(true) {};
        //inline optional_ref_(T const& ref) : ptr(new T(ref)), reallocated(true) {};
        inline optional_ref_() {};

        // check operator
        inline operator bool() { return !!this->ptr; };
        inline operator bool() const { return !!this->ptr; };

        // type conversion
        inline operator T& () { return *this->ptr; };
        inline operator T const& () const { return *this->ptr; };

        // type conversion
        inline operator T* () { return this->ptr; };
        inline operator T const* () const { return this->ptr; };

        // assign optional const
        inline decltype(auto) operator=(std::optional<T> const& ref) { if (ref) { pointerAssign(ptr, ref.value()); }; return *this; };
        inline decltype(auto) operator=(optional_ref<T> const& ref) { if (ref) { pointerReAssign(ptr, ref.value()); }; return *this; };

        // assign optional
        inline decltype(auto) operator=(std::optional<T>& ref) { if (ref) { pointerAssign(ptr, ref.value()); }; return *this; };
        inline decltype(auto) operator=(optional_ref<T>& ref) { if (ref) { pointerReAssign(ptr, ref.value()); }; return *this; };

        // assign reference
        inline decltype(auto) operator=(T const& ref) { pointerAssign(ptr, ref); return *this; };
        inline decltype(auto) operator=(T& ref) { pointerAssign(ptr, ref); return *this; };

        // value alias
        inline decltype(auto) value() { return *this->ptr; };
        inline decltype(auto) value() const { return *this->ptr; };

        // value alias
        [[deprecated]] inline decltype(auto) ref() { return *this->ptr; };
        [[deprecated]] inline decltype(auto) ref() const { return *this->ptr; };

        // accessing operator
        inline decltype(auto) operator *() { return *this->ptr; };
        inline decltype(auto) operator *() const { return *this->ptr; };

        // const accessing operator
        inline decltype(auto) operator ->() { return this->ptr; };
        inline decltype(auto) operator ->() const { return this->ptr; };

        // because it's reference, pointer must got directly...
        inline decltype(auto) operator&() { return this->ptr; };
        inline decltype(auto) operator&() const { return this->ptr; };

        // proxy...
        inline decltype(auto) operator[](uintptr_t const& index) { return (*this->ptr)[index]; };
        inline decltype(auto) operator[](uintptr_t const& index) const { return (*this->ptr)[index]; };
    };

    // 
    template<class T = void_t>
    inline decltype(auto) opt_ref(T& ref) {
        //using T = std::decay_t<decltype(ref)>;
        return optional_ref<T>(ref);
    };

    // 
    template<class T = void_t>
    inline decltype(auto) opt_cref(const T& ref) {
        using T = const std::decay_t<decltype(ref)>;
        return optional_ref<T>(ref);
    };

    //
    template<class Ts = void_t>
    inline decltype(auto) pointer(optional_ref<Ts> ref) {
        Ts* pt = ref ? (&ref.value()) : nullptr;
        return wrap_ptr<Ts>(pt);
    };

    //
    template<class Ts = void_t>
    inline decltype(auto) pointer(optional_ref<const Ts> ref) {
        Ts* pt = ref ? (&ref.value()) : nullptr;
        return wrap_ptr<Ts>(pt);
    };

    //
    template<class Ts = void_t>
    inline decltype(auto) pointer(std::optional<Ts>& ref) {
        Ts* pt = ref ? (&ref.value()) : nullptr;
        return wrap_ptr<Ts>(pt);
    };

};
#endif
