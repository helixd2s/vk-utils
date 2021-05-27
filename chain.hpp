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
#include <cppcoro/generator.hpp>
#include <cgranges/cpp/IITree.h>

//#include <soa/vector.hpp>

#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS

#ifdef _WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#else

#endif

// 
#include <vulkan/vulkan.hpp>
#include <vulkan-utils/sizes.h>

// 
namespace vku {

    // 
    namespace unsafe {

        //
        template<class O = vk::BaseOutStructure>
        inline auto searchPtr(const vk::StructureType& sType, O& base, type_safe::optional_ref<vk::BaseOutStructure*&> parent = {}) {
            auto pNext = &base;
            auto structure = type_safe::opt_ref<vk::BaseOutStructure*&>(reinterpret_cast<vk::BaseOutStructure*&>(reinterpret_cast<vk::BaseOutStructure&>(base).pNext));//what
            auto last = type_safe::opt_ref<vk::BaseOutStructure*&>(reinterpret_cast<vk::BaseOutStructure*&>(pNext));
            type_safe::optional_ref<vk::BaseOutStructure*&> found = {};
            if (parent) { parent.value() = last.value(); };

            //
            while (bool(structure) && bool(structure.value())) {
                last = structure;
                if (parent) { parent.value() = last.value(); };
                if (structure.value()->sType == sType) { found = structure; };
                structure = type_safe::opt_ref<vk::BaseOutStructure*&>(reinterpret_cast<vk::BaseOutStructure*&>(structure.value()->pNext));
            };

            //
            return found;
        };

        // for Vulkan Hpp only!
        template<class O = vk::BaseOutStructure>
        inline auto relocate(const vk::StructureType& sType, O& base, void* const& where = nullptr) {
            vk::BaseOutStructure* last = nullptr;
            auto found = searchPtr(sType, base, type_safe::opt_ref<vk::BaseOutStructure*&>(last));

            //
            if (where) {
                if (found) 
                    { memcpy(where, found.value(), vkGetStructureSizeBySType(VkStructureType(sType))); found.value() = reinterpret_cast<vk::BaseOutStructure* const&>(where); } else
                    { /*found = where;*/ last->pNext = found.value(); };
            };

            //
            return found;
        };

        // for Vulkan Hpp only!
        template<class O = vk::BaseOutStructure>
        inline auto relocate(const vk::StructureType& sType, O& base, type_safe::optional_ref<void*> where = {}) {
            vk::BaseOutStructure* last = nullptr;
            auto found = searchPtr(sType, base, type_safe::opt_ref<vk::BaseOutStructure*&>(last));

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
            auto found = relocate<O>(sType, base, type_safe::opt_ref(const_cast<void*&>(where)));
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
        inline auto chainify(const vk::StructureType& sType, O& base, const void* what = nullptr, type_safe::optional_ref<void*> where = {}) {
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
            auto structure = type_safe::opt_ref(*reinterpret_cast<const vk::BaseInStructure*&>(*reinterpret_cast<const vk::BaseInStructure&>(base).pNext));//what
            auto last = structure;
            type_safe::optional_ref<const T*> found = {};

            //
            while (structure && *structure) {
                last = structure;
                if (structure->sType == sType) { found == type_safe::opt_ref(reinterpret_cast<const T*&>(*structure)); };
                structure = type_safe::opt_ref(*reinterpret_cast<const vk::BaseInStructure*&>(structure->pNext));
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
        inline auto relocate(O& base, type_safe::optional_ref<void*> where = {}) { return relocate<O>(sType, base, where); };

        //
        template<vk::StructureType sType, class O = vk::BaseOutStructure>
        inline auto chainify(O& base, const void* what = nullptr, void* const& where = nullptr) { return chainify<O>(sType, base, what, where); };

        //
        template<vk::StructureType sType, class O = vk::BaseOutStructure>
        inline auto chainify(O& base, const void* what = nullptr, type_safe::optional_ref<void*> where = nullptr) { return chainify<O>(sType, base, what, where); };

        //
        template<vk::StructureType sType, class O = vk::BaseOutStructure, class T = vk::BaseInStructure>
        inline auto fromChain(const O& base) { return fromChain<O, T>(sType, base); };
    };


    // 
    class ChainStorage : public std::enable_shared_from_this<ChainStorage> { 
    protected: 
        std::vector<std::shared_ptr<vk::BaseOutStructure>> storage = {};

    public: 
        template<class T = vk::BaseOutStructure>
        inline auto [[nodiscard]] getElement(const vk::StructureType& sType) {
            std::shared_ptr<T> element = {};
            for (auto& e : storage) { if (e->sType == sType) { element = std::reinterpret_pointer_cast<T>(e); }; };
            return element;
        };

        template<class T = vk::BaseOutStructure>
        inline std::shared_ptr<ChainStorage> addElement(const T& structure = T{}, const uintptr_t size = sizeof(T)) {
            auto exist = this->getElement<T>();
            if (!exist) {
                std::shared_ptr<vk::BaseOutStructure> bk = storage.size() > 0 ? storage.back() : std::shared_ptr<vk::BaseOutStructure>{};
                storage.push_back(std::shared_ptr<vk::BaseOutStructure>((vk::BaseOutStructure*)malloc(size), free));
                exist = std::reinterpret_pointer_cast<T>(storage.back());
                if (bk) { bk->pNext = exist.get(); };
                *exist = structure;
            } else {
                const auto sp = exist->pNext; // TODO: use correct pNext
                *exist = structure;
                exist->pNext = sp; // TODO: use correct pNext
            };
            return shared_from_this();
        };

        template<class T = vk::BaseOutStructure>
        inline std::shared_ptr<ChainStorage> copyChain(const T& base = T{}) {
            auto structure = type_safe::opt_ref(*reinterpret_cast<const vk::BaseInStructure*&>(*reinterpret_cast<const vk::BaseInStructure&>(base).pNext));//what
            auto last = structure;
            while (structure && *structure) {
                last = structure;
                this->addElement(reinterpret_cast<const vk::BaseOutStructure&>(*structure), vkGetStructureSizeBySType(structure->sType));
                structure = type_safe::opt_ref(*reinterpret_cast<const vk::BaseInStructure*&>(structure->pNext));
            };
            return shared_from_this();
        };
    };


};
