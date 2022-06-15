#pragma once 

//
#ifdef __cplusplus
#include "./core.hpp"

// 
#ifdef VKU_ENABLE_INTERVAL
#include <interval-tree/interval_tree.hpp>
#endif

// 
namespace cpp21 {

#ifdef VKU_ENABLE_INTERVAL
    // 
    template<
        class N,
        class T,
        template<class Ns = N, class Ts = T> class Um = std::unordered_map
    > class interval_map_;
    template<
        class N,
        class T,
        template<class Ns = N, class Ts = T> class Um = std::unordered_map
    > using interval_map = interval_map_<decay_t<N>, decay_t<T>, Um>;

    // 
    template<
        class N,
        class T,
        template<class Ns, class Ts> class Um
    >
    class interval_map_ {
    public:
        Um<N, T> map = {};
        lib_interval_tree::interval_tree_t<N> intervals = {};

    public:
        // 
        inline decltype(auto) insert(lib_interval_tree::interval<N, lib_interval_tree::closed> const& interval, T const& obj) {
            auto it = intervals.overlap_find({ interval.low(), interval.low() });
            if (map.find(interval.low()) == map.end()) { map[interval.low()] = obj; };
            if (it != intervals.end()) { intervals.insert(interval); intervals.deoverlap(); };
            return this;
        };

        // 
        inline decltype(auto) erase(N const& address = 0ull) {
            auto it = intervals.overlap_find({ address, address });
            if (it != intervals.end()) {
                auto mt = map.find(it->interval().low());
                map.erase(mt);
                intervals.erase(it);
            };
            return this;
        };

        // 
        inline decltype(auto) find(N const& address = 0ull) {
            auto it = intervals.overlap_find({ address, address });
            return it != intervals.end() ? opt_ref<T>(map.at(it->interval().low())) : optional_ref<T>{};
        };

        // 
        inline decltype(auto) find(N const& address = 0ull) const {
            auto& intervals = const_cast<lib_interval_tree::interval_tree_t<N>&>(this->intervals);
            auto it = intervals.overlap_find({ address, address });
            return it != intervals.end() ? opt_cref<T>(map.at(it->interval().low())) : optional_ref<const T>{};
        };
    };
#endif

};
#endif