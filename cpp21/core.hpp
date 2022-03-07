#pragma once

// 
#include <memory>
#include <vector>
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

// 
#ifdef VKU_ENABLE_TYPE_SAFE
#include <type_safe/optional_ref.hpp>
#include <type_safe/reference.hpp>
#include <type_safe/integer.hpp>
#endif

// 
namespace cpp21 {

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

  // make short alias
#ifdef VKU_ENABLE_TYPE_SAFE
  namespace ts {
    using namespace type_safe;
  };
#endif

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
  enum default_t : uint32_t {

  };

  // only for construct vulkan structures
  //template<class T>
  inline decltype(auto) copy_as_shared(auto const* data) {
    using T = std::decay_t<decltype(data)>;
    auto shared = std::shared_ptr<T>((T*)calloc(1ull,sizeof(T)), free);
    memcpy(shared.get(), data, sizeof(T));
    //*shared = data; // what if structure can self-copy?
    return shared;
  };

  // 
  inline decltype(auto) copy_as_shared(auto const& data) {
    using T = std::decay_t<decltype(data)>;
    auto shared = std::shared_ptr<T>((T*)calloc(1ull,sizeof(T)), free);
    //memcpy(shared.get(), data, sizeof(T));
    *shared = data; // what if structure can self-copy?
    return shared;
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
  template<class T = void_t>
  class wrap_ptr;

  //
  template<class T, template<class Ts = T> class Sp = std::shared_ptr, template<class Ts = T> class W = wrap_ptr>
  class wrap_shared_ptr;

  //
#pragma pack(0)
  __declspec(align(0)) class void_t;




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

};