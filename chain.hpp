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
#include "./sizes.h"
#include "./cpp21.hpp"

// 
namespace vku {

    // 
#ifdef VKU_ENABLE_TYPE_SAFE
    namespace ts {
        using namespace type_safe;
    };
#endif

#ifdef TYPE_SAFE_OPTIONAL_REF_HPP_INCLUDED
    template<class T> using optional_ref = ts::optional_ref<T>;
    template<class T> using opt_ref = ts::opt_ref<T>;
    template<class T> using opt_cref = ts::opt_cref<T>;
#else 
    template<class T> using optional_ref = cpp21::optional_ref<T>;
    template<class T> using opt_ref = cpp21::opt_ref<T>;
    template<class T> using opt_cref = cpp21::opt_cref<T>;
#endif

    // 
    namespace unsafe {

        // TODO unified class, if this doesn't working
        template<class St = VkStructureType>
        using VkStructureTypeT = St;

        //
        //template<class O = VkBaseOutStructure, class St = VkStructureType>
        template<class St = VkStructureType>
        inline decltype(auto) searchPtr(const St& sType, auto& base = VkBaseOutStructure{.sType = reinterpret_cast<VkStructureType>(sType), .pNext = nullptr}, optional_ref<stm::wrap_ptr<VkBaseOutStructure>> parent = {}) {
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
        template<class St = VkStructureType>
        inline decltype(auto) relocate(St const& sType, auto& base = VkBaseOutStructure{.sType = reinterpret_cast<VkStructureType>(sType), .pNext = nullptr}, void* const& where = nullptr) {
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
        template<class St = VkStructureType>
        inline decltype(auto) relocate(St const& sType, auto& base = VkBaseOutStructure{.sType = reinterpret_cast<VkStructureType>(sType), .pNext = nullptr}, optional_ref<stm::wrap_ptr<stm::void_t>> where = {}) {
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
        template<class St = VkStructureType>
        inline decltype(auto) chainify(St const& sType, auto& base = VkBaseOutStructure{.sType = reinterpret_cast<VkStructureType>(sType), .pNext = nullptr}, const void* what = nullptr, void* const& where = nullptr) {
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
        template<class St = VkStructureType>
        inline decltype(auto) chainify(St const& sType, O& base = VkBaseOutStructure{.sType = reinterpret_cast<VkStructureType>(sType), .pNext = nullptr}, const void* what = nullptr, optional_ref<stm::wrap_ptr<stm::void_t>> where = {}) {
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
        template<class T = VkBaseInStructure, class St = VkStructureType>
        inline decltype(auto) fromChain(St const& sType, auto const& base = VkBaseOutStructure{.sType = reinterpret_cast<VkStructureType>(sType), .pNext = nullptr}) {
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
        template<VkStructureTypeT sType>
        inline decltype(auto) searchPtr(auto& base = VkBaseOutStructure{.sType = reinterpret_cast<VkStructureType>(sType), .pNext = nullptr}) { return searchPtr(sType, base); };

        //
        template<VkStructureTypeT sType>
        inline decltype(auto) relocate(auto& base = VkBaseOutStructure{.sType = reinterpret_cast<VkStructureType>(sType), .pNext = nullptr}, void* const& where = nullptr) { return relocate(sType, base, where); };

        //
        template<VkStructureTypeT sType>
        inline decltype(auto) relocate(auto& base = VkBaseOutStructure{.sType = reinterpret_cast<VkStructureType>(sType), .pNext = nullptr}, optional_ref<stm::wrap_ptr<stm::void_t>> where = {}) { return relocate(sType, base, where); };

        //
        template<VkStructureTypeT sType>
        inline decltype(auto) chainify(auto& base = VkBaseOutStructure{.sType = reinterpret_cast<VkStructureType>(sType), .pNext = nullptr}, const void* what = nullptr, void* const& where = nullptr) { return chainify(sType, base, what, where); };

        //
        template<VkStructureTypeT sType>
        inline decltype(auto) chainify(auto& base = VkBaseOutStructure{.sType = reinterpret_cast<VkStructureType>(sType), .pNext = nullptr}, const void* what = nullptr, optional_ref<stm::wrap_ptr<stm::void_t>> where = nullptr) { return chainify(sType, base, what, where); };

        //
        template<VkStructureTypeT sType, class T = VkBaseInStructure>
        inline decltype(auto) fromChain(const auto& base = VkBaseOutStructure{.sType = reinterpret_cast<VkStructureType>(sType), .pNext = nullptr}) { return fromChain<T>(sType, base); };
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
        decltype(auto) getLeader() {
            if (storage.size() > 0) { return storage[0];} else { return {}; };
        };

        // 
        decltype(auto) getLeader() const {
            if (storage.size() > 0) { return storage[0]; } else { return {}; };
        };

        template<class T = VkBaseOutStructure, class St = VkStructureType>
        inline decltype(auto) [[nodiscard]] getElement(St const& sType) {
            std::shared_ptr<T> element = {};
            for (auto& e : storage) { if (e->sType == sType) { element = std::reinterpret_pointer_cast<T>(e); break; }; };
            return element;
        };

        template<class T = VkBaseOutStructure, class St = VkStructureType>
        inline decltype(auto) [[nodiscard]] getReference(St const& sType) {
            auto element = this->getElement<T>(sType);
            return element ? opt_ref<T>(*element) : optional_ref<T>{};
        };

        template<class T = VkBaseInStructure>
        inline decltype(auto) addElement(const T& structure = T{}, const uintptr_t size = sizeof(T)) {
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
        inline decltype(auto) copyChain(const T& base = T{}) {
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
