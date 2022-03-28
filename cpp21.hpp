#pragma once

//
#ifdef __cplusplus
#include "./cpp21/core.hpp"
#include "./cpp21/void.hpp"
#include "./cpp21/bool32.hpp"
#include "./cpp21/wrap_ptr.hpp"
#include "./cpp21/data_view.hpp"
#include "./cpp21/optional_ref.hpp"
#include "./cpp21/bucket.hpp"
#include "./cpp21/memory_stack.hpp"
#include "./cpp21/of_shared.hpp"
#include "./cpp21/shared_of.hpp"

// 
#define VKU_ENABLE_INTERVAL

// 
#ifdef VKU_ENABLE_INTERVAL
#include "./cpp21/interval_map.hpp"
#endif

// 
namespace cpp21 {
};
#endif
