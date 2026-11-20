#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>

#include "kalaime/layout-api/phonetic_layout.hpp"

namespace kalaime::layout {

class LayoutRegistry {
   public:
    LayoutRegistry() = default;
    LayoutRegistry(const LayoutRegistry&) = delete;
    LayoutRegistry& operator=(const LayoutRegistry&) = delete;

    bool register_layout(std::string_view id, std::unique_ptr<PhoneticLayout> layout);
    bool set_active(std::string_view id) noexcept;
    const PhoneticLayout* find(std::string_view id) const noexcept;
    const PhoneticLayout* active() const noexcept;
    std::string_view active_id() const noexcept;

   private:
    std::unordered_map<std::string, std::unique_ptr<PhoneticLayout>> layouts_;
    std::string active_id_;
};

}  // namespace kalaime::layout
