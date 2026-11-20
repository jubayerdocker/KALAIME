#include "kalaime/layout-api/layout_registry.hpp"

#include <utility>

namespace kalaime::layout {

bool LayoutRegistry::register_layout(std::string_view id, std::unique_ptr<PhoneticLayout> layout) {
    if (id.empty() || layout == nullptr) {
        return false;
    }
    const std::string key(id);
    if (layouts_.contains(key)) {
        return false;
    }
    layouts_.emplace(key, std::move(layout));
    if (active_id_.empty()) {
        active_id_ = key;
    }
    return true;
}

bool LayoutRegistry::set_active(std::string_view id) noexcept {
    const std::string key(id);
    if (!layouts_.contains(key)) {
        return false;
    }
    active_id_ = key;
    return true;
}

const PhoneticLayout* LayoutRegistry::find(std::string_view id) const noexcept {
    const auto iterator = layouts_.find(std::string(id));
    return iterator == layouts_.end() ? nullptr : iterator->second.get();
}

const PhoneticLayout* LayoutRegistry::active() const noexcept {
    return active_id_.empty() ? nullptr : find(active_id_);
}

std::string_view LayoutRegistry::active_id() const noexcept { return active_id_; }

}  // namespace kalaime::layout
