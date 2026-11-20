#include "suborno/core/engine.hpp"

#include "suborno/dictionary/dictionary.hpp"
#include "suborno/layouts/national/national_layout.hpp"
#include "suborno/layouts/phonetic/suborno_phonetic_layout.hpp"
#include "suborno/layouts/probhat/probhat_layout.hpp"
#include "suborno/settings/settings.hpp"

namespace suborno::core {

Engine::Engine() {
    layouts_.register_layout("suborno-phonetic", std::make_unique<layouts::SubornoPhoneticLayout>(),
                             layout::LayoutMetadata{"suborno-phonetic", "Suborno Phonetic", "1"});
    layouts_.register_layout("probhat", std::make_unique<layouts::ProbhatLayout>(),
                             layout::LayoutMetadata{"probhat", "Probhat", "1"});
    layouts_.register_layout("national", std::make_unique<layouts::NationalLayout>(),
                             layout::LayoutMetadata{"national", "National", "1"});
}

Engine::Engine(const EngineConfig& config) : Engine() {
    if (!config.settings_path.empty()) {
        settings_store_ = std::make_unique<settings::SettingsStore>(config.settings_path);
        const settings::SettingsLoadResult result = settings_store_->load();
        if (result.status == settings::SettingsStatus::ok) {
            current_settings_ = result.settings;
        }
    }
    if (!config.dictionary_path.empty()) {
        dictionary_ = std::make_unique<dictionary::Dictionary>(config.dictionary_path.string());
    }
}

Engine::~Engine() = default;

bool Engine::register_layout(std::string_view id, std::unique_ptr<layout::PhoneticLayout> layout) {
    return layouts_.register_layout(id, std::move(layout));
}

bool Engine::set_active_layout(std::string_view id) noexcept { return layouts_.set_active(id); }

std::string_view Engine::active_layout_id() const noexcept { return layouts_.active_id(); }

const layout::LayoutMetadata* Engine::layout_metadata(std::string_view id) const noexcept {
    return layouts_.find_metadata(id);
}

std::unique_ptr<layouts::PhoneticInputSession> Engine::create_session() {
    const layout::PhoneticLayout* active_layout = layouts_.active();
    if (active_layout == nullptr) {
        return nullptr;
    }
    if (dictionary_ != nullptr) {
        return std::make_unique<layouts::PhoneticInputSession>(*active_layout, *dictionary_);
    }
    return std::make_unique<layouts::PhoneticInputSession>(*active_layout);
}

settings::SettingsStatus Engine::save_settings(const settings::EngineSettings& settings) {
    if (settings_store_ == nullptr) {
        return settings::SettingsStatus::io_error;
    }
    const settings::SettingsStatus status = settings_store_->save(settings);
    if (status == settings::SettingsStatus::ok) {
        current_settings_ = settings;
        if (layouts_.find(settings.active_layout_id) != nullptr) {
            layouts_.set_active(settings.active_layout_id);
        }
    }
    return status;
}

settings::SettingsLoadResult Engine::load_settings() const {
    if (settings_store_ == nullptr) {
        settings::SettingsLoadResult result;
        result.settings = current_settings_;
        result.status = settings::SettingsStatus::ok;
        return result;
    }
    return settings_store_->load();
}

const settings::EngineSettings& Engine::settings() const noexcept { return current_settings_; }

dictionary::Dictionary* Engine::dictionary() noexcept { return dictionary_.get(); }

const dictionary::Dictionary* Engine::dictionary() const noexcept { return dictionary_.get(); }

}  // namespace suborno::core
