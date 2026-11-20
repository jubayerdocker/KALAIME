#pragma once

#include <memory>
#include <string_view>

#include "kalaime/layout-api/layout_registry.hpp"
#include "kalaime/layouts/phonetic/phonetic_input_session.hpp"

namespace kalaime::core {

class Engine {
   public:
    Engine();

    bool register_layout(std::string_view id, std::unique_ptr<layout::PhoneticLayout> layout);
    bool set_active_layout(std::string_view id) noexcept;
    std::string_view active_layout_id() const noexcept;

    std::unique_ptr<layouts::PhoneticInputSession> create_session() const;

   private:
    layout::LayoutRegistry layouts_;
};

}  // namespace kalaime::core
