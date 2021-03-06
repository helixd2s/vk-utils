#pragma once

//
#ifdef __cplusplus
#ifdef CPP21_DEBUG_MEMORY
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

//
#ifdef _DEBUG
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
// Replace _NORMAL_BLOCK with _CLIENT_BLOCK if you want the
// allocations to be of _CLIENT_BLOCK type
#else
#define DBG_NEW new
#endif
#endif

// 
#ifdef USE_CMAKE_PCH
#include <cmake_pch.hpp>
#else
#include <vector>
#include <string>
#include <memory>
#include <string_view>
#include <utility>
#include <map>
#include <future>
#include <iostream>
#include <optional>
#include <coroutine>
#include <tuple>
#include <type_traits>
#include <utility>
#include <unordered_map>
#include <iterator>
#include <fstream>
#include <typeindex>
#include <coroutine>
#include <span>
#include <numeric>
#endif

// 
namespace cpp21 {

    template<class T> inline T& unmove(T&& t) { return t; }

    //
    template<typename T>
    inline void apply_permutation(std::vector<T>& v, std::vector<uintptr_t> const& order) {
        for (int s = 1, d; s < order.size(); ++s) {
            for (d = order[s]; d < s; d = order[d]);
            if (d == s) while (d = order[d], d != s) std::swap(v[s], v[d]);
        };
    };


    //
    template<typename T> struct is_shared_ptr : std::false_type {};
    template<typename T> struct is_shared_ptr<std::shared_ptr<T>> : std::true_type {};

    template<typename T> struct is_vector : public std::false_type {};

    template<typename T, typename A>
    struct is_vector<std::vector<T, A>> : public std::true_type {};

    template <typename T> using static_not = std::integral_constant<bool, !T::value>;
    //
    //template<typename T> struct shared_ptr_t {
      //using St = std::shared_ptr<T>;
      //static constexpr decltype(auto) type(St _s) { return *_s; };
    //};

    // 
#define CPP21_FN_ALIAS(NAME, CALL) \
  template <typename... Args>\
  inline decltype(auto) NAME(Args&&... args) {\
    return CALL(args...);\
  };

  // aggregate cache
    inline auto* cache = new unsigned char[256u * 256u];

    // 
    inline decltype(auto) aggregate(auto const& str) {
        using T = std::decay_t<decltype(str)>;
        memcpy(cache, &str, sizeof(T));
        return reinterpret_cast<T&>(*cache);
    };

    // 
    inline decltype(auto) aggregate(auto const& str, auto& cache_pointer) {
        using T = std::decay_t<decltype(str)>;
        memcpy(cache, &str, sizeof(T));
        return reinterpret_cast<T&>(*cache);
    };

    // 
    inline decltype(auto) unlock32(auto& cache) {
        return reinterpret_cast<uint32_t&>(cache);
    };

    // 
    inline decltype(auto) unlock32(auto const& cache) {
        return reinterpret_cast<uint32_t const&>(cache);
    };

    // 
    inline decltype(auto) zero32(auto& cache) {
        return (reinterpret_cast<uint32_t&>(cache) = 0u);
    };

    // 
    inline decltype(auto) unlock64(auto& cache) {
        return reinterpret_cast<uint64_t&>(cache);
    };

    // 
    inline decltype(auto) unlock64(auto const& cache) {
        return reinterpret_cast<uint64_t const&>(cache);
    };

    // 
    inline decltype(auto) zero64(auto& cache) {
        return (reinterpret_cast<uint64_t&>(cache) = 0u);
    };

    //
    template<typename T>
    inline decltype(auto) bytesize(std::vector<T> const& vec) {
        return sizeof(T) * vec.size();
    };

    //
    enum default_t : uint32_t {

    };

    // only for construct vulkan structures
    //template<class T>
    inline decltype(auto) copy_as_shared(auto const* data) {
        using T = std::decay_t<decltype(data)>;
        return std::make_shared<T>(std::move(*data));
    };

    // 
    inline decltype(auto) copy_as_shared(auto const& data) {
        using T = std::decay_t<decltype(data)>;
        return std::make_shared<T>(std::move(data));
    };

    //
    template<class T = std::string>
    inline decltype(auto) toCString(std::vector<char const*>& cVect, std::vector<T> const& Vs = {}) {
        cVect.resize(Vs.size());
        std::transform(Vs.begin(), Vs.end(), cVect.begin(), [](std::string& str) { return str.c_str(); });
        return cVect;
    };

    //
    using localptr_t = uintptr_t;


    //#ifdef USE_VULKAN
    static inline decltype(auto) sgn(auto const& val) { using T = std::decay_t<decltype(val)>; return (T(0) < val) - (val < T(0)); }
    static inline decltype(auto) tiled(auto const& sz, auto const& gmaxtile) {
        // return (int32_t)ceil((double)sz / (double)gmaxtile);
        return sz <= 0 ? 0 : (sz / gmaxtile + sgn(sz % gmaxtile));
    };

    // 
    template <class T> static inline decltype(auto) strided(size_t const& size = 1u) { return sizeof(T) * size; };
    template <class T> static inline decltype(auto) makeVector(T const* ptr, size_t const& size = 1) { std::vector<T>v(size); memcpy(v.data(), ptr, strided<T>(size)); return v; };

    // 
    template<class Ts>
    inline decltype(auto) vector_cast(auto const& Vy) {
        std::vector<Ts> V{};
        for (decltype(auto) v : Vy) { V.push_back(reinterpret_cast<Ts const&>(v)); };
        return std::move(V);
    };

    // 
    inline decltype(auto) shift(auto* data, uintptr_t offset = 0ull) { return reinterpret_cast<decltype(data)>(reinterpret_cast<uintptr_t&>(data) + offset); };
    inline decltype(auto) shift(const auto* data, uintptr_t offset = 0ull) { return reinterpret_cast<decltype(data)>(reinterpret_cast<const uintptr_t&>(data) + offset); };

    //
    template<class T>
    inline decltype(auto) exchange(T& ref, T const& value) { auto old = ref; ref = value; return old; };

    //using void_t = uint8_t;
#pragma pack(push, 0)
    class void_t {
    public:
        //uint8_t : 0;

        // 
        inline void_t() {};
        inline void_t(auto const& data) { this->assign(data); };

        //
        inline decltype(auto) assign(auto const* obj) { using Ts = std::decay_t<decltype(obj)>; memcpy(this, obj, sizeof(Ts)); return reinterpret_cast<Ts&>(*this); };
        inline decltype(auto) assign(auto const& obj) { return this->assign(&obj); };

        //
        inline decltype(auto) operator=(auto const& obj) { return this->assign(obj); };
        inline decltype(auto) operator=(auto const* obj) { return this->assign(obj); };

        // 
        template<class Ts = void_t> inline decltype(auto) operator->() { return this->get<Ts>(); };
        template<class Ts = void_t> inline decltype(auto) operator->() const { return this->get<Ts>(); };

        // 
        template<class Ts = void_t> inline decltype(auto) operator *() { return *this->get<Ts>(); };
        template<class Ts = void_t> inline decltype(auto) operator *() const { return *this->get<Ts>(); };

        // 
        template<class Ts = void_t> inline operator Ts& () { return *this->get<Ts>(); };
        template<class Ts = void_t> inline operator Ts const& () const { return *this->get<Ts>(); };

        // 
        template<class Ts = void_t> inline operator Ts* () { return this->get<Ts>(); };
        template<class Ts = void_t> inline operator Ts const* () const { return this->get<Ts>(); };

        // 
        template<class Ts = void_t> inline decltype(auto) get() { return reinterpret_cast<Ts*>(this); };
        template<class Ts = void_t> inline decltype(auto) get() const { return reinterpret_cast<Ts const*>(this); };

        // 
        inline operator bool() const { return false; };
    };
#pragma pack(pop)

    //
    template<class T = void_t> using decay_t = std::remove_cv_t<std::decay_t<std::remove_cv_t<T>>>;


    //
    template<class T = void_t>
    void _free(void* ptr_) {
        if (ptr_) { delete reinterpret_cast<T*>(ptr_); };
    };

    //
    template<class Tt = void_t, class Tf = void_t>
    void _memcpy(void* _to, void const* _from, size_t _size) {
        (*reinterpret_cast<Tt*>(_to)) = (*reinterpret_cast<Tf const*>(_from));
    };

    //
    template<class T>
    void* _malloc(size_t _size) {
        // TODO: correct division
        return (new T[_size / sizeof(T)]);
    };

    //
    template<class E = void_t>
    class self_copy_intrusive;

    // 
    template<class T = void_t, template<class Es = void_t> class Iw = self_copy_intrusive>
    class self_copy_intrusive_t;

    // 
    template<class T = void_t, template<class Ts = T> class I = self_copy_intrusive_t>
    class self_copy_ptr;

    //
    template<class T = void_t, template<class Ts = T> class Vt = std::vector, template<class Sv = Vt<T>> class Sh = std::shared_ptr>
    using shared_vector_t = std::shared_ptr<Vt<T>>;

    //
    template<class K = uintptr_t, class T = void_t, template<class Ks = K, class Ts = T> class Um = std::unordered_map, template<class Sm = Um<K, T>> class Sh = std::shared_ptr>
    using shared_map_t = Sh<Um<K, T>>;


    //
  //#pragma pack(1)
    //__declspec(align(1)) class void_t;

    //
    template<typename R>
    inline bool is_ready(std::future<R> const& f)
    {
        return f.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
    }

    //
    template<class T, uint32_t count = 16u>
    class limited_list {
    public:
        std::list<T> queue = {};

        inline decltype(auto) push_back(T const& e) {
            if (queue.size() >= count) { queue.pop_front(); };
            queue.push_back(e);
            return this;
        };

        inline decltype(auto) back() { return queue.back(); };
        inline decltype(auto) back() const { return queue.back(); };
        inline decltype(auto) size() const { return queue.size(); };
    };

    // 
    static inline decltype(auto) readBinaryChar(std::string const& filePath) { // open the file:
        std::vector<char> vec{};
        decltype(auto) file = std::ifstream(filePath, std::ios::in | std::ios::binary | std::ios::ate);
        if (file.is_open()) { // Stop eating new lines in binary mode!!!
            file.unsetf(std::ios::skipws);

            // get its size:
            std::streampos fileSize = 0ull;
            file.seekg(0, std::ios::end);
            fileSize = file.tellg();
            file.seekg(0, std::ios::beg);

            // reserve capacity
            vec.reserve(fileSize);
            vec.insert(vec.begin(), std::istream_iterator<char>(file), std::istream_iterator<char>());
        }
        else {
            std::cerr << "Failure to open " + filePath << std::endl;
        };
        if (vec.size() < 1u) { std::cerr << "NO DATA " + filePath << std::endl; };
        return vec;
    };

    // 
    static inline decltype(auto) readBinaryU32(std::string const& filePath) {
        const auto vect8u = readBinaryChar(filePath);
        auto vect32 = std::vector<uint32_t>(tiled(uint64_t(vect8u.size()), uint64_t(4ull)));
        memcpy(vect32.data(), vect8u.data(), vect8u.size());
        return vect32;
    };


    template<class T>
    bool orEqual(T const& initial, std::vector<T> const& equals = {}) {
        bool equally = false;
        for (decltype(auto) value : equals) { equally |= (initial == value); };
        return equally;
    };

    template<class T>
    bool andEqual(T const& initial, std::vector<T> const& equals = {}) {
        bool equally = true;
        for (decltype(auto) value : equals) { equally &= (initial == value); };
        return equally;
    };


    template<class T>
    T& pointerReAssign(T*& ptr, T const& val) {
        if (!ptr) { ptr = const_cast<T*>(&val); }
        else { *ptr = val; }; return *ptr;
    };

    template<class T>
    T& pointerReAssign(T*& ptr, T& val) {
        if (!ptr) { ptr = &val; }
        else { *ptr = val; }; return *ptr;
    };

    template<class T>
    T& pointerReAssign(T*& ptr, T&& val) {
        if (!ptr) { ptr = &unmove(val); }
        else { *ptr = val; }; return *ptr;
    };

    template<class T>
    T& pointerAssign(T* const& ptr, T& val) {
        *ptr = val; return *ptr;
    };

    template<class T>
    T& pointerAssign(T* const& ptr, T const& val) {
        *ptr = val; return *ptr;
    };

    template<class T>
    T& pointerAssign(T* const& ptr, T&& val) {
        *ptr = val; return *ptr;
    };


    //
    template<typename T>
    inline std::string to_hex_string(T n)
    {
        std::stringstream stream;
        stream << std::setfill('0') << std::setw(2 * sizeof(T)) << std::hex << n;
        return stream.str();
    }
};

#endif