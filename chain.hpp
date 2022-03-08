#pragma once

// 
#include <memory>
#include <vector>
#include <map>
#include <future>
#include <iostream>
#include <optional>
#include <coroutine>

// 
#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS

//
#ifdef _WIN32
#ifndef VK_USE_PLATFORM_WIN32_KHR
#define VK_USE_PLATFORM_WIN32_KHR
#include <windows.h>
#endif
#else
#ifdef __linux__ 
//FD defaultly
#endif
#endif

// 
#include "./cpp21.hpp"
#include "./sizes.h"

// 
#include <vulkan/vulkan.h>

// 
namespace vku {

  //
  struct EmptyChain {
    VkStructureType sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    const void* pNext = nullptr;
    uint8_t _dataPack[1024u] = { 0 };
  };

  //
  template<class A = VkAccessFlagBits2, class I = VkImageLayout>
  inline decltype(auto) getCorrectAccessMaskByImageLayout(I const& imageLayout_) {
    VkImageLayout imageLayout = reinterpret_cast<VkImageLayout const&>(imageLayout_);
    VkAccessFlagBits2 accessMask = {};

    // 
    switch (imageLayout) {
    case VK_IMAGE_LAYOUT_GENERAL:
      accessMask |= VK_ACCESS_2_MEMORY_READ_BIT | VK_ACCESS_2_MEMORY_WRITE_BIT;
      break;

    case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
      accessMask |= VK_ACCESS_2_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
      break;

    case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
      accessMask |= VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
      break;

    case VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL:
      accessMask |= VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
      break;

    case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
      accessMask |= VK_ACCESS_2_SHADER_READ_BIT;
      break;

    case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
      accessMask |= VK_ACCESS_2_TRANSFER_READ_BIT;
      break;

    case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
      accessMask |= VK_ACCESS_2_TRANSFER_WRITE_BIT;
      break;

      // there is no correct conversion
    case VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL:
      accessMask |= VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
      break;

      // there is no correct conversion
    case VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL:
      accessMask |= VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
      break;

      // there is no correct conversion
    case VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL:
      accessMask |= VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
      break;

      // there is no correct conversion
    case VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL:
      accessMask |= VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
      break;

      // there is no correct conversion
    case VK_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL:
      accessMask |= VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
      break;

      // there is no correct conversion
    case VK_IMAGE_LAYOUT_STENCIL_READ_ONLY_OPTIMAL:
      accessMask |= VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
      break;

      //
    case VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL:
      accessMask |= VK_ACCESS_2_MEMORY_READ_BIT;
      break;

      //
    case VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL:
      accessMask |= VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT | VK_ACCESS_2_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
      break;

      //
    case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
      accessMask |= VK_ACCESS_2_MEMORY_READ_BIT;
      break;

    default:;

      // TODO: video decoding and encoding API
    };

    return A(accessMask);
  };


  //
  template<class P = VkPipelineStageFlagBits2, class A = VkAccessFlagBits2>
  inline decltype(auto) getCorrectPipelineStagesByAccessMask(A const& accessMask_) {
    VkAccessFlagBits2 accessMask = reinterpret_cast<VkAccessFlagBits2 const&>(accessMask_);
    VkPipelineStageFlagBits2 pipelineStageMask = {};

    // 
    for (uint32_t i = 0u; i < 32u; i++) {
      const auto masked = accessMask & (1 << i);
      switch (masked) {
      case VK_ACCESS_2_MEMORY_READ_BIT:
        pipelineStageMask |= VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
        break;

      case VK_ACCESS_2_MEMORY_WRITE_BIT:
        pipelineStageMask |= VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
        break;

      case VK_ACCESS_2_INDIRECT_COMMAND_READ_BIT:
        pipelineStageMask |= VK_PIPELINE_STAGE_2_DRAW_INDIRECT_BIT;
        break;

      case VK_ACCESS_2_INDEX_READ_BIT:
        pipelineStageMask |= VK_PIPELINE_STAGE_2_INDEX_INPUT_BIT;
        break;

      case VK_ACCESS_2_VERTEX_ATTRIBUTE_READ_BIT:
        pipelineStageMask |= VK_PIPELINE_STAGE_2_VERTEX_ATTRIBUTE_INPUT_BIT;
        break;

      case VK_ACCESS_2_INPUT_ATTACHMENT_READ_BIT:
        pipelineStageMask |= VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT;
        break;

      case VK_ACCESS_2_COLOR_ATTACHMENT_READ_BIT:
        pipelineStageMask |= VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
        break;

      case VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT:
        pipelineStageMask |= VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
        break;

      case VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT:
        pipelineStageMask |= VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT;
        break;

      case VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT:
        pipelineStageMask |= VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT;
        break;

      case VK_ACCESS_2_TRANSFER_READ_BIT:
        pipelineStageMask |= VK_PIPELINE_STAGE_2_COPY_BIT | VK_PIPELINE_STAGE_2_BLIT_BIT | VK_PIPELINE_STAGE_2_RESOLVE_BIT;
        break;

      case VK_ACCESS_2_TRANSFER_WRITE_BIT:
        pipelineStageMask |= VK_PIPELINE_STAGE_2_COPY_BIT | VK_PIPELINE_STAGE_2_BLIT_BIT | VK_PIPELINE_STAGE_2_CLEAR_BIT | VK_PIPELINE_STAGE_2_RESOLVE_BIT;
        break;

      case VK_ACCESS_2_HOST_READ_BIT:
        pipelineStageMask |= VK_PIPELINE_STAGE_2_HOST_BIT;
        break;

      case VK_ACCESS_2_HOST_WRITE_BIT:
        pipelineStageMask |= VK_PIPELINE_STAGE_2_HOST_BIT;
        break;

      case VK_ACCESS_2_CONDITIONAL_RENDERING_READ_BIT_EXT:
        pipelineStageMask |= VK_PIPELINE_STAGE_2_CONDITIONAL_RENDERING_BIT_EXT;
        break;

      case VK_ACCESS_2_TRANSFORM_FEEDBACK_WRITE_BIT_EXT:
        pipelineStageMask |= VK_PIPELINE_STAGE_2_TRANSFORM_FEEDBACK_BIT_EXT;
        break;

      case VK_ACCESS_2_TRANSFORM_FEEDBACK_COUNTER_READ_BIT_EXT:
        pipelineStageMask |= VK_PIPELINE_STAGE_2_TRANSFORM_FEEDBACK_BIT_EXT;
        break;

      case VK_ACCESS_2_TRANSFORM_FEEDBACK_COUNTER_WRITE_BIT_EXT:
        pipelineStageMask |= VK_PIPELINE_STAGE_2_TRANSFORM_FEEDBACK_BIT_EXT;
        break;

      case VK_ACCESS_2_COMMAND_PREPROCESS_READ_BIT_NV:
        pipelineStageMask |= VK_PIPELINE_STAGE_2_COMMAND_PREPROCESS_BIT_NV;
        break;

      case VK_ACCESS_2_COMMAND_PREPROCESS_WRITE_BIT_NV:
        pipelineStageMask |= VK_PIPELINE_STAGE_2_COMMAND_PREPROCESS_BIT_NV;
        break;

      case VK_ACCESS_2_COLOR_ATTACHMENT_READ_NONCOHERENT_BIT_EXT:
        pipelineStageMask |= VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
        break;

      case VK_ACCESS_2_INVOCATION_MASK_READ_BIT_HUAWEI:
        pipelineStageMask |= VK_PIPELINE_STAGE_2_INVOCATION_MASK_BIT_HUAWEI;
        break;

      case VK_ACCESS_2_ACCELERATION_STRUCTURE_READ_BIT_KHR:
        pipelineStageMask |= VK_PIPELINE_STAGE_2_RAY_TRACING_SHADER_BIT_KHR | VK_PIPELINE_STAGE_2_ACCELERATION_STRUCTURE_BUILD_BIT_KHR;
        break;

      case VK_ACCESS_2_ACCELERATION_STRUCTURE_WRITE_BIT_KHR:
        pipelineStageMask |= VK_PIPELINE_STAGE_2_ACCELERATION_STRUCTURE_BUILD_BIT_KHR;
        break;

      case VK_ACCESS_2_FRAGMENT_DENSITY_MAP_READ_BIT_EXT:
        pipelineStageMask |= VK_PIPELINE_STAGE_2_FRAGMENT_DENSITY_PROCESS_BIT_EXT;
        break;

      case VK_ACCESS_2_FRAGMENT_SHADING_RATE_ATTACHMENT_READ_BIT_KHR:
        pipelineStageMask |= VK_PIPELINE_STAGE_2_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;
        break;

      default:;

        // TODO: video decoding and encoding API
      };
    };

    return P(pipelineStageMask);
  };


  // 
#ifdef VKU_ENABLE_TYPE_SAFE
  namespace ts {
    using namespace type_safe;
  };
#endif

#ifdef TYPE_SAFE_OPTIONAL_REF_HPP_INCLUDED
  template<class T = cpp21::void_t> using optional_ref = ts::optional_ref<T>;
  CPP21_FN_ALIAS(opt_ref, ts::opt_ref);
  CPP21_FN_ALIAS(opt_cref, ts::opt_cref);
#else 
  template<class T = cpp21::void_t> using optional_ref = cpp21::optional_ref<T>;
  //inline constexpr decltype(auto) opt_ref = cpp21::opt_ref;
  //inline constexpr decltype(auto) opt_cref = cpp21::opt_cref;
  CPP21_FN_ALIAS(opt_ref, cpp21::opt_ref);
  CPP21_FN_ALIAS(opt_cref, cpp21::opt_cref);
#endif

  // 
  namespace unsafe {

    // TODO unified class, if this doesn't working
    template<class St = VkStructureType>
    using VkStructureTypeT = St;

    //
    //template<class O = VkBaseOutStructure, class St = VkStructureType>
    template<class St = VkStructureType>
    inline decltype(auto) searchPtr(const St& sType, auto& base = VkBaseOutStructure{ .sType = reinterpret_cast<VkStructureType>(sType), .pNext = nullptr }, optional_ref<cpp21::wrap_ptr<VkBaseOutStructure>> parent = {}) {
      auto pNext = &base;
      auto structure = opt_ref(reinterpret_cast<cpp21::wrap_ptr<VkBaseOutStructure>&>(reinterpret_cast<VkBaseOutStructure&>(base).pNext));//what
      auto last = opt_ref(reinterpret_cast<cpp21::wrap_ptr<VkBaseOutStructure>&>(pNext));
      optional_ref<cpp21::wrap_ptr<VkBaseOutStructure>> found = {};
      if (parent) { parent.value() = last.value(); };

      //
      while (bool(structure) && bool(structure.value())) {
        last = structure;
        if (parent) { parent.value() = last.value(); };
        if (structure.value()->sType == sType) { found = structure; };
        structure = opt_ref(reinterpret_cast<cpp21::wrap_ptr<VkBaseOutStructure>&>(structure.value()->pNext));
      };

      //
      return found;
    };

    // for Vulkan Hpp only!
    template<class St = VkStructureType>
    inline decltype(auto) relocate(St const& sType, auto& base = VkBaseOutStructure{ .sType = reinterpret_cast<VkStructureType>(sType), .pNext = nullptr }, void* const& where = nullptr) {
      cpp21::wrap_ptr<VkBaseOutStructure> last = nullptr;
      auto found = searchPtr(sType, base, opt_ref<cpp21::wrap_ptr<VkBaseOutStructure>>(last));

      //
      if (where) {
        if (found)
        {
          memcpy(where, found.value(), vkGetStructureSizeBySType(reinterpret_cast<VkStructureType const&>(sType))); found.value() = reinterpret_cast<cpp21::wrap_ptr<VkBaseOutStructure> const&>(where);
        }
        else
        { /*found = where;*/ last->pNext = reinterpret_cast<VkBaseOutStructure* const&>(where);
        };
      };

      //
      return found;
    };

    // for Vulkan Hpp only!
    template<class St = VkStructureType>
    inline decltype(auto) relocate(St const& sType, auto& base = VkBaseOutStructure{ .sType = reinterpret_cast<VkStructureType>(sType), .pNext = nullptr }, optional_ref<cpp21::wrap_ptr<cpp21::void_t>> where = {}) {
      cpp21::wrap_ptr<VkBaseOutStructure> last = nullptr;
      auto found = searchPtr(sType, base, opt_ref(last));

      //
      if (where) {
        if (found)
        {
          memcpy(where, *found, vkGetStructureSizeBySType(reinterpret_cast<VkStructureType const&>(sType))); *found = where;
        }
        else
        {
          found = where; last->pNext = found;
        };
      };

      //
      return found;
    };

    //
    template<class St = VkStructureType>
    inline decltype(auto) chainify(St const& sType, auto& base = VkBaseOutStructure{ .sType = reinterpret_cast<VkStructureType>(sType), .pNext = nullptr }, const void* what = nullptr, void* const& where = nullptr) {
      auto found = relocate(sType, base, opt_ref(reinterpret_cast<cpp21::wrap_ptr<cpp21::void_t>&>(const_cast<void*&>(where))));
      if (what && found && *found) {
        auto& typed = reinterpret_cast<VkBaseOutStructure*&>(found.value());
        const auto pNext = typed->pNext;
        memcpy(typed, what, vkGetStructureSizeBySType(reinterpret_cast<VkStructureType const&>(sType)));
        typed->pNext = pNext;
      };
      return found;
    };

    //
    template<class St = VkStructureType>
    inline decltype(auto) chainify(St const& sType, auto& base = VkBaseOutStructure{ .sType = reinterpret_cast<VkStructureType>(sType), .pNext = nullptr }, const void* what = nullptr, optional_ref<cpp21::wrap_ptr<cpp21::void_t>> where = {}) {
      auto found = relocate(sType, base, where);
      if (what && found && *found) {
        auto& typed = reinterpret_cast<VkBaseOutStructure*&>(found.value());
        const auto pNext = typed->pNext;
        memcpy(typed, what, vkGetStructureSizeBySType(reinterpret_cast<VkStructureType const&>(sType)));
        typed->pNext = pNext;
      };
      return found;
    };

    // for Vulkan Hpp only!
    //template<class T, class O = VkBaseInStructure>
    template<class T = VkBaseInStructure, class St = VkStructureType>
    inline decltype(auto) fromChain(St const& sType, auto const& base = VkBaseOutStructure{ .sType = reinterpret_cast<St const&>(sType), .pNext = nullptr }) {
      auto& ptr = reinterpret_cast<const cpp21::wrap_ptr<const VkBaseInStructure>&>(reinterpret_cast<const VkBaseInStructure&>(base).pNext);
      auto structure = ptr ? opt_cref(*ptr) : optional_ref<const VkBaseInStructure>{};//what
      auto last = structure;
      optional_ref<const T> found = {};

      //
      while (structure) {
        last = structure;
        if (structure.value().sType == sType) { found = opt_cref(reinterpret_cast<T&>(structure.value())); };
        structure = structure.value().pNext ? opt_ref(*reinterpret_cast<const cpp21::wrap_ptr<VkBaseInStructure>&>(structure.value().pNext)) : optional_ref<const VkBaseInStructure>{};
      };

      //
      return found;
    };

    /* // NOT POSSIBLE!
    //
    template<VkStructureTypeT sType>
    inline decltype(auto) searchPtr(auto& base = VkBaseOutStructure{.sType = reinterpret_cast<VkStructureType>(sType), .pNext = nullptr}) { return searchPtr(sType, base); };

    //
    template<VkStructureTypeT sType>
    inline decltype(auto) relocate(auto& base = VkBaseOutStructure{.sType = reinterpret_cast<VkStructureType>(sType), .pNext = nullptr}, void* const& where = nullptr) { return relocate(sType, base, where); };

    //
    template<VkStructureTypeT sType>
    inline decltype(auto) relocate(auto& base = VkBaseOutStructure{.sType = reinterpret_cast<VkStructureType>(sType), .pNext = nullptr}, optional_ref<cpp21::wrap_ptr<cpp21::void_t>> where = {}) { return relocate(sType, base, where); };

    //
    template<VkStructureTypeT sType>
    inline decltype(auto) chainify(auto& base = VkBaseOutStructure{.sType = reinterpret_cast<VkStructureType>(sType), .pNext = nullptr}, const void* what = nullptr, void* const& where = nullptr) { return chainify(sType, base, what, where); };

    //
    template<VkStructureTypeT sType>
    inline decltype(auto) chainify(auto& base = VkBaseOutStructure{.sType = reinterpret_cast<VkStructureType>(sType), .pNext = nullptr}, const void* what = nullptr, optional_ref<cpp21::wrap_ptr<cpp21::void_t>> where = nullptr) { return chainify(sType, base, what, where); };

    //
    template<VkStructureTypeT sType, class T = VkBaseInStructure>
    inline decltype(auto) fromChain(const auto& base = VkBaseOutStructure{.sType = reinterpret_cast<VkStructureType>(sType), .pNext = nullptr}) { return fromChain<T>(sType, base); };
    */
  };

  // 
  //[[deprecated]]
  class ChainStorage : public std::enable_shared_from_this<ChainStorage> {
  protected:
    std::vector<std::shared_ptr<VkBaseOutStructure>> storage = {};

  public:
    //
    ChainStorage() {

    };

    //
    ChainStorage(VkBaseInStructure const& structure, uintptr_t const& size = 0u) {
      auto sized = size > 0 ? size : vkGetStructureSizeBySType((VkStructureType const&)structure.sType);
      auto memory = std::shared_ptr<VkBaseOutStructure>((VkBaseOutStructure*)calloc(sized,1ull), free); // simpler to allocate new
      memcpy(memory.get(), &structure, size); // simpler to do memcpy
      storage = std::vector<std::shared_ptr<VkBaseOutStructure>>{ memory }; // simpler reconstruct 
    };

    // 
    decltype(auto) getLeader() { return storage[0]; };
    decltype(auto) getLeader() const { return storage[0]; };

    //
    template<class T = VkBaseOutStructure, class St = VkStructureType>
    inline decltype(auto) [[nodiscard]] getElement(St const& sType) {
      std::shared_ptr<T> element = {};
      for (auto& e : storage) { if (e->sType == sType) { element = std::reinterpret_pointer_cast<T>(e); break; }; };
      return element;
    };

    // 
    template<class T = VkBaseOutStructure, class St = VkStructureType>
    inline decltype(auto) [[nodiscard]] getReference(St const& sType) {
      auto element = this->getElement<T>(sType);
      return element ? opt_ref<T>(*element) : optional_ref<T>{};
    };

    //
    template<class T = VkBaseInStructure>
    inline decltype(auto) addElement(const T& structure = T{}, const uintptr_t size = sizeof(T)) {
      auto exist = this->getElement<T>(structure.sType);
      auto sized = size > 0 ? size : vkGetStructureSizeBySType(reinterpret_cast<VkStructureType const&>(structure.sType));
      if (!exist) {
        std::shared_ptr<VkBaseOutStructure> bk = storage.size() > 0 ? storage.back() : std::shared_ptr<VkBaseOutStructure>{};
        storage.push_back(std::shared_ptr<VkBaseOutStructure>((VkBaseOutStructure*)calloc(sized,1ull), free));
        exist = std::reinterpret_pointer_cast<T>(storage.back());
        if (bk) { bk->pNext = reinterpret_cast<VkBaseOutStructure*>(exist.get()); };
        memcpy(exist.get(), &structure, size); // size is important!
        //*exist = structure; // you idiot?
        //exist->pNext = nullptr; // needs to nullify pNext
      }
      else {
        const auto sp = exist->pNext; // TODO: use correct pNext
        //*exist = structure; // you idiot?
        memcpy(exist.get(), &structure, size); // size is important!
        exist->pNext = sp; // TODO: use correct pNext
      };
      return shared_from_this();
    };

    // 
    template<class T = VkBaseOutStructure>
    inline decltype(auto) copyChain(const T& base = T{}) {
      auto& ptr = reinterpret_cast<const cpp21::wrap_ptr<const VkBaseInStructure>&>(reinterpret_cast<const VkBaseInStructure&>(base).pNext);
      auto structure = ptr ? opt_cref(*ptr) : optional_ref<const VkBaseInStructure>{};//what
      auto last = structure;

      //
      this->addElement(base);
      while (structure) {
        last = structure;
        this->addElement(reinterpret_cast<const VkBaseOutStructure&>(structure.value()), vkGetStructureSizeBySType(reinterpret_cast<VkStructureType const&>(structure.value().sType)));
        structure = structure.value().pNext ? opt_cref(*reinterpret_cast<const cpp21::wrap_ptr<VkBaseInStructure>&>(structure.value().pNext)) : optional_ref<const VkBaseInStructure>{};
      };

      return shared_from_this();
    };
  };


};
