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
#include <type_safe/optional_ref.hpp>
#include <type_safe/reference.hpp>

#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS

#ifdef _WIN32
#ifndef VK_USE_PLATFORM_WIN32_KHR
#define VK_USE_PLATFORM_WIN32_KHR
#endif
#else

#endif

// 
#include <vulkan/vulkan.hpp>
#include <vulkan-utils/sizes.h>

//
#include "./link.hpp"

// 
namespace vku {

    namespace ts {
        using namespace type_safe;
    };

    // 
    namespace unsafe {

        //
        template<class O = vk::BaseOutStructure>
        inline auto searchPtr(const vk::StructureType& sType, O& base, ts::optional_ref<stm::wrap_ptr<vk::BaseOutStructure>> parent = {}) {
            auto pNext = &base;
            auto structure = ts::opt_ref(reinterpret_cast<stm::wrap_ptr<vk::BaseOutStructure>&>(reinterpret_cast<vk::BaseOutStructure&>(base).pNext));//what
            auto last = ts::opt_ref(reinterpret_cast<stm::wrap_ptr<vk::BaseOutStructure>&>(pNext));
            ts::optional_ref<stm::wrap_ptr<vk::BaseOutStructure>> found = {};
            if (parent) { parent.value() = last.value(); };

            //
            while (bool(structure) && bool(structure.value())) {
                last = structure;
                if (parent) { parent.value() = last.value(); };
                if (structure.value()->sType == sType) { found = structure; };
                structure = ts::opt_ref(reinterpret_cast<stm::wrap_ptr<vk::BaseOutStructure>&>(structure.value()->pNext));
            };

            //
            return found;
        };

        // for Vulkan Hpp only!
        template<class O = vk::BaseOutStructure>
        inline auto relocate(const vk::StructureType& sType, O& base, void* const& where = nullptr) {
            stm::wrap_ptr<vk::BaseOutStructure> last = nullptr;
            auto found = searchPtr(sType, base, ts::opt_ref<stm::wrap_ptr<vk::BaseOutStructure>>(last));

            //
            if (where) {
                if (found) 
                    { memcpy(where, found.value(), vkGetStructureSizeBySType(VkStructureType(sType))); found.value() = reinterpret_cast<stm::wrap_ptr<vk::BaseOutStructure> const&>(where); } else
                    { /*found = where;*/ last->pNext = reinterpret_cast<vk::BaseOutStructure* const&>(where); };
            };

            //
            return found;
        };

        // for Vulkan Hpp only!
        template<class O = vk::BaseOutStructure>
        inline auto relocate(const vk::StructureType& sType, O& base, ts::optional_ref<stm::wrap_ptr<stm::void_t>> where = {}) {
            stm::wrap_ptr<vk::BaseOutStructure> last = nullptr;
            auto found = searchPtr(sType, base, ts::opt_ref(last));

            //
            if (where) {
                if (found) 
                    { memcpy(where, *found, vkGetStructureSizeBySType(VkStructureType(sType))); *found = where; } else
                    { found = where; last->pNext = found; };
            };

            //
            return found;
        };

        //
        template<class O = vk::BaseOutStructure>
        inline auto chainify(const vk::StructureType& sType, O& base, const void* what = nullptr, void* const& where = nullptr) {
           auto found = relocate<O>(sType, base, ts::opt_ref(reinterpret_cast<stm::wrap_ptr<stm::void_t>&>(const_cast<void*&>(where))));
            if (what && found && *found) {
                auto& typed = reinterpret_cast<vk::BaseOutStructure*&>(found.value());
                const auto pNext = typed->pNext;
                memcpy(typed, what, vkGetStructureSizeBySType(VkStructureType(sType)));
                typed->pNext = pNext;
            };
            return found;
        };

        //
        template<class O = vk::BaseOutStructure>
        inline auto chainify(const vk::StructureType& sType, O& base, const void* what = nullptr, ts::optional_ref<stm::wrap_ptr<stm::void_t>> where = {}) {
            auto found = relocate<O>(sType, base, where);
            if (what && found && *found) {
                auto& typed = reinterpret_cast<vk::BaseOutStructure*&>(found.value());
                const auto pNext = typed->pNext;
                memcpy(typed, what, vkGetStructureSizeBySType(VkStructureType(sType)));
                typed->pNext = pNext;
            };
            return found;
        };

        // for Vulkan Hpp only!
        //template<class T, class O = vk::BaseInStructure>
        template<class O = vk::BaseInStructure, class T = vk::BaseInStructure>
        inline auto fromChain(const vk::StructureType& sType, const O& base) {
            auto& ptr = reinterpret_cast<const stm::wrap_ptr<const vk::BaseInStructure>&>(reinterpret_cast<const vk::BaseInStructure&>(base).pNext);
            auto structure = ptr ? ts::opt_cref(*ptr) : ts::optional_ref<const vk::BaseInStructure>{};//what
            auto last = structure;
            ts::optional_ref<const T> found = {};

            //
            while (structure) {
                last = structure;
                if (structure.value().sType == sType) { found = ts::opt_cref(reinterpret_cast<T&>(structure.value())); };
                structure = structure.value().pNext ? ts::opt_ref(*reinterpret_cast<const stm::wrap_ptr<vk::BaseInStructure>&>(structure.value().pNext)) : ts::optional_ref<const vk::BaseInStructure>{};
            };

            //
            return found;
        };


        //
        template<vk::StructureType sType, class O = vk::BaseOutStructure>
        inline auto searchPtr(O& base) { return searchPtr<O>(sType, base); };

        //
        template<vk::StructureType sType, class O = vk::BaseOutStructure>
        inline auto relocate(O& base, void* const& where = nullptr) { return relocate<O>(sType, base, where); };

        //
        template<vk::StructureType sType, class O = vk::BaseOutStructure>
        inline auto relocate(O& base, ts::optional_ref<stm::wrap_ptr<stm::void_t>> where = {}) { return relocate<O>(sType, base, where); };

        //
        template<vk::StructureType sType, class O = vk::BaseOutStructure>
        inline auto chainify(O& base, const void* what = nullptr, void* const& where = nullptr) { return chainify<O>(sType, base, what, where); };

        //
        template<vk::StructureType sType, class O = vk::BaseOutStructure>
        inline auto chainify(O& base, const void* what = nullptr, ts::optional_ref<stm::wrap_ptr<stm::void_t>> where = nullptr) { return chainify<O>(sType, base, what, where); };

        //
        template<vk::StructureType sType, class O = vk::BaseOutStructure, class T = vk::BaseInStructure>
        inline auto fromChain(const O& base) { return fromChain<O, T>(sType, base); };
    };


    // 
    class ChainStorage : public std::enable_shared_from_this<ChainStorage> { 
    protected: 
        std::vector<std::shared_ptr<vk::BaseOutStructure>> storage = {};

    public: 
        //
        ChainStorage() {
        
        };

        //
        ChainStorage(vk::BaseInStructure const& structure, uintptr_t const& size = 0u) {
            auto sized = size > 0 ? size : vkGetStructureSizeBySType((VkStructureType const&)structure.sType);
            auto memory = std::shared_ptr<vk::BaseOutStructure>((vk::BaseOutStructure*)malloc(sized), free); // simpler to allocate new
            memcpy(memory.get(), &structure, size); // simpler to do memcpy
            storage = std::vector<std::shared_ptr<vk::BaseOutStructure>>{ memory }; // simpler reconstruct 
        };

        // 
        std::shared_ptr<vk::BaseOutStructure> getLeader() {
            return storage[0];
        };

        template<class T = vk::BaseOutStructure>
        inline auto [[nodiscard]] getElement(const vk::StructureType& sType) {
            std::shared_ptr<T> element = {};
            for (auto& e : storage) { if (e->sType == sType) { element = std::reinterpret_pointer_cast<T>(e); break; }; };
            return element;
        };

        template<class T = vk::BaseOutStructure>
        inline auto [[nodiscard]] getReference(const vk::StructureType& sType) {
            auto element = this->getElement<T>(sType);
            return element ? ts::opt_ref<T>(*element) : ts::optional_ref<T>{};
        };

        template<class T = vk::BaseInStructure>
        inline std::shared_ptr<ChainStorage> addElement(const T& structure = T{}, const uintptr_t size = sizeof(T)) {
            auto exist = this->getElement<T>(structure.sType);
            auto sized = size > 0 ? size : vkGetStructureSizeBySType((VkStructureType const&)structure.sType);
            if (!exist) {
                std::shared_ptr<vk::BaseOutStructure> bk = storage.size() > 0 ? storage.back() : std::shared_ptr<vk::BaseOutStructure>{};
                storage.push_back(std::shared_ptr<vk::BaseOutStructure>((vk::BaseOutStructure*)malloc(sized), free));
                exist = std::reinterpret_pointer_cast<T>(storage.back());
                if (bk) { bk->pNext = reinterpret_cast<vk::BaseOutStructure*>(exist.get()); };
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

        template<class T = vk::BaseOutStructure>
        inline std::shared_ptr<ChainStorage> copyChain(const T& base = T{}) {
            auto& ptr = reinterpret_cast<const stm::wrap_ptr<const vk::BaseInStructure>&>(reinterpret_cast<const vk::BaseInStructure&>(base).pNext);
            auto structure = ptr ? ts::opt_cref(*ptr) : ts::optional_ref<const vk::BaseInStructure>{};//what
            auto last = structure;

            //
            this->addElement(base);
            while (structure) {
                last = structure;
                this->addElement(reinterpret_cast<const vk::BaseOutStructure&>(structure.value()), vkGetStructureSizeBySType(VkStructureType(structure.value().sType)));
                structure = structure.value().pNext ? ts::opt_cref(*reinterpret_cast<const stm::wrap_ptr<vk::BaseInStructure>&>(structure.value().pNext)) : ts::optional_ref<const vk::BaseInStructure>{};
            };

            return shared_from_this();
        };
    };


};
