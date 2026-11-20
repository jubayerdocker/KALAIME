#include "kalaime/core/engine.hpp"

#include "kalaime/layouts/phonetic/kala_phonetic_layout.hpp"

namespace kalaime::core {

Engine::Engine() {
    layouts_.register_layout("kala-phonetic", std::make_unique<layouts::KalaPhoneticLayout>());
}

bool Engine::register_layout(std::string_view id, std::unique_ptr<layout::PhoneticLayout> layout) {
    return layouts_.register_layout(id, std::move(layout));
}

bool Engine::set_active_layout(std::string_view id) noexcept { return layouts_.set_active(id); }

std::string_view Engine::active_layout_id() const noexcept { return layouts_.active_id(); }

std::unique_ptr<layouts::PhoneticInputSession> Engine::create_session() const {
    const layout::PhoneticLayout* active_layout = layouts_.active();
    if (active_layout == nullptr) {
        return nullptr;
    }
    return std::make_unique<layouts::PhoneticInputSession>(*active_layout);
}

}  // namespace kalaime::core
