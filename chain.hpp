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
#endif
#else

#endif

// 
#include <vulkan/vulkan.h>
#include <vulkan-utils/sizes.h>

//
#include "./utils.hpp"

// 
namespace vku {

    // 
#ifdef VKU_ENABLE_TYPE_SAFE
    namespace ts {
        using namespace type_safe;
    };
#endif

#ifdef TYPE_SAFE_OPTIONAL_REF_HPP_INCLUDED
    using optional_ref = ts::optional_ref;
    using opt_ref = ts::opt_ref;
    using opt_cref = ts::opt_cref;
#else 
    using optional_ref = stm::optional_ref;
    using opt_ref = stm::opt_ref;
    using opt_cref = stm::opt_cref;
#endif

    // 
    namespace unsafe {

        // TODO unified class, if this doesn't working
        template<class St = VkStructureType>
        using VkStructureTypeT = St;

        //
        template<class O = VkBaseOutStructure, class St = VkStructureType>
        inline auto searchPtr(const St& sType, O& base, optional_ref<stm::wrap_ptr<VkBaseOutStructure>> parent = {}) {
            auto pNext = &base;
            auto structure = opt_ref(reinterpret_cast<stm::wrap_ptr<VkBaseOutStructure>&>(reinterpret_cast<VkBaseOutStructure&>(base).pNext));//what
            auto last = opt_ref(reinterpret_cast<stm::wrap_ptr<VkBaseOutStructure>&>(pNext));
            optional_ref<stm::wrap_ptr<VkBaseOutStructure>> found = {};
            if (parent) { parent.value() = last.value(); };

            //
            while (bool(structure) && bool(structure.value())) {
                last = structure;
                if (parent) { parent.value() = last.value(); };
                if (structure.value()->sType == sType) { found = structure; };
                structure = opt_ref(reinterpret_cast<stm::wrap_ptr<VkBaseOutStructure>&>(structure.value()->pNext));
            };

            //
            return found;
        };

        // for Vulkan Hpp only!
        template<class O = VkBaseOutStructure, class St = VkStructureType>
        inline auto relocate(St const& sType, O& base, void* const& where = nullptr) {
            stm::wrap_ptr<VkBaseOutStructure> last = nullptr;
            auto found = searchPtr(sType, base, opt_ref<stm::wrap_ptr<VkBaseOutStructure>>(last));

            //
            if (where) {
                if (found) 
                    { memcpy(where, found.value(), vkGetStructureSizeBySType(reinterpret_cast<VkStructureType const&>(sType))); found.value() = reinterpret_cast<stm::wrap_ptr<VkBaseOutStructure> const&>(where); } else
                    { /*found = where;*/ last->pNext = reinterpret_cast<VkBaseOutStructure* const&>(where); };
            };

            //
            return found;
        };

        // for Vulkan Hpp only!
        template<class O = VkBaseOutStructure, class St = VkStructureType>
        inline auto relocate(St const& sType, O& base, optional_ref<stm::wrap_ptr<stm::void_t>> where = {}) {
            stm::wrap_ptr<VkBaseOutStructure> last = nullptr;
            auto found = searchPtr(sType, base, opt_ref(last));

            //
            if (where) {
                if (found) 
                    { memcpy(where, *found, vkGetStructureSizeBySType(reinterpret_cast<VkStructureType const&>(sType))); *found = where; } else
                    { found = where; last->pNext = found; };
            };

            //
            return found;
        };

        //
        template<class O = VkBaseOutStructure, class St = VkStructureType>
        inline auto chainify(St const& sType, O& base, const void* what = nullptr, void* const& where = nullptr) {
           auto found = relocate<O>(sType, base, opt_ref(reinterpret_cast<stm::wrap_ptr<stm::void_t>&>(const_cast<void*&>(where))));
            if (what && found && *found) {
                auto& typed = reinterpret_cast<VkBaseOutStructure*&>(found.value());
                const auto pNext = typed->pNext;
                memcpy(typed, what, vkGetStructureSizeBySType(reinterpret_cast<VkStructureType const&>(sType)));
                typed->pNext = pNext;
            };
            return found;
        };

        //
        template<class O = VkBaseOutStructure, class St = VkStructureType>
        inline auto chainify(St const& sType, O& base, const void* what = nullptr, optional_ref<stm::wrap_ptr<stm::void_t>> where = {}) {
            auto found = relocate<O>(sType, base, where);
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
        template<class O = VkBaseInStructure, class T = VkBaseInStructure>
        inline auto fromChain(St const& sType, O const& base) {
            auto& ptr = reinterpret_cast<const stm::wrap_ptr<const VkBaseInStructure>&>(reinterpret_cast<const VkBaseInStructure&>(base).pNext);
            auto structure = ptr ? opt_cref(*ptr) : optional_ref<const VkBaseInStructure>{};//what
            auto last = structure;
            optional_ref<const T> found = {};

            //
            while (structure) {
                last = structure;
                if (structure.value().sType == sType) { found = opt_cref(reinterpret_cast<T&>(structure.value())); };
                structure = structure.value().pNext ? opt_ref(*reinterpret_cast<const stm::wrap_ptr<VkBaseInStructure>&>(structure.value().pNext)) : optional_ref<const VkBaseInStructure>{};
            };

            //
            return found;
        };

        //
        template<VkStructureTypeT sType, class O = VkBaseOutStructure>
        inline auto searchPtr(O& base) { return searchPtr<O>(sType, base); };

        //
        template<VkStructureTypeT sType, class O = VkBaseOutStructure>
        inline auto relocate(O& base, void* const& where = nullptr) { return relocate<O>(sType, base, where); };

        //
        template<VkStructureTypeT sType, class O = VkBaseOutStructure>
        inline auto relocate(O& base, optional_ref<stm::wrap_ptr<stm::void_t>> where = {}) { return relocate<O>(sType, base, where); };

        //
        template<VkStructureTypeT sType, class O = VkBaseOutStructure>
        inline auto chainify(O& base, const void* what = nullptr, void* const& where = nullptr) { return chainify<O>(sType, base, what, where); };

        //
        template<VkStructureTypeT sType, class O = VkBaseOutStructure>
        inline auto chainify(O& base, const void* what = nullptr, optional_ref<stm::wrap_ptr<stm::void_t>> where = nullptr) { return chainify<O>(sType, base, what, where); };

        //
        template<VkStructureTypeT sType, class O = VkBaseOutStructure, class T = VkBaseInStructure>
        inline auto fromChain(const O& base) { return fromChain<O, T>(sType, base); };
    };

    // 
    [[deprecated]]
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
            auto memory = std::shared_ptr<VkBaseOutStructure>((VkBaseOutStructure*)malloc(sized), free); // simpler to allocate new
            memcpy(memory.get(), &structure, size); // simpler to do memcpy
            storage = std::vector<std::shared_ptr<VkBaseOutStructure>>{ memory }; // simpler reconstruct 
        };

        // 
        std::shared_ptr<VkBaseOutStructure> getLeader() {
            if (storage.size() > 0) { return storage[0];} else { return {}; };
        };

        // 
        std::shared_ptr<VkBaseOutStructure> getLeader() const {
            if (storage.size() > 0) { return storage[0]; } else { return {}; };
        };

        template<class T = VkBaseOutStructure, class St = VkStructureType>
        inline auto [[nodiscard]] getElement(St const& sType) {
            std::shared_ptr<T> element = {};
            for (auto& e : storage) { if (e->sType == sType) { element = std::reinterpret_pointer_cast<T>(e); break; }; };
            return element;
        };

        template<class T = VkBaseOutStructure, class St = VkStructureType>
        inline auto [[nodiscard]] getReference(St const& sType) {
            auto element = this->getElement<T>(sType);
            return element ? opt_ref<T>(*element) : optional_ref<T>{};
        };

        template<class T = VkBaseInStructure>
        inline std::shared_ptr<ChainStorage> addElement(const T& structure = T{}, const uintptr_t size = sizeof(T)) {
            auto exist = this->getElement<T>(structure.sType);
            auto sized = size > 0 ? size : vkGetStructureSizeBySType(reinterpret_cast<VkStructureType const&>(structure.sType));
            if (!exist) {
                std::shared_ptr<VkBaseOutStructure> bk = storage.size() > 0 ? storage.back() : std::shared_ptr<VkBaseOutStructure>{};
                storage.push_back(std::shared_ptr<VkBaseOutStructure>((VkBaseOutStructure*)malloc(sized), free));
                exist = std::reinterpret_pointer_cast<T>(storage.back());
                if (bk) { bk->pNext = reinterpret_cast<VkBaseOutStructure*>(exist.get()); };
                memcpy(exist.get(), &structure, size); // size is important!
                //*exist = structure; // you idiot?
                //exist->pNext = nullptr; // needs to nullify pNext
            } else {
                const auto sp = exist->pNext; // TODO: use correct pNext
                //*exist = structure; // you idiot?
                memcpy(exist.get(), &structure, size); // size is important!
                exist->pNext = sp; // TODO: use correct pNext
            };
            return shared_from_this();
        };

        template<class T = VkBaseOutStructure>
        inline std::shared_ptr<ChainStorage> copyChain(const T& base = T{}) {
            auto& ptr = reinterpret_cast<const stm::wrap_ptr<const VkBaseInStructure>&>(reinterpret_cast<const VkBaseInStructure&>(base).pNext);
            auto structure = ptr ? opt_cref(*ptr) : optional_ref<const VkBaseInStructure>{};//what
            auto last = structure;

            //
            this->addElement(base);
            while (structure) {
                last = structure;
                this->addElement(reinterpret_cast<const VkBaseOutStructure&>(structure.value()), vkGetStructureSizeBySType(reinterpret_cast<VkStructureType const&>(structure.value().sType)));
                structure = structure.value().pNext ? opt_cref(*reinterpret_cast<const stm::wrap_ptr<VkBaseInStructure>&>(structure.value().pNext)) : optional_ref<const VkBaseInStructure>{};
            };

            return shared_from_this();
        };
    };


};
