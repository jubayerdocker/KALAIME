#include "kalaime/settings/settings.hpp"

#include <fstream>
#include <system_error>

namespace kalaime::settings {
namespace {

constexpr int kSettingsVersion = 1;

bool parse_uint(const std::string& value, std::size_t& result) {
    if (value.empty()) {
        return false;
    }
    std::size_t parsed = 0;
    for (const char character : value) {
        if (character < '0' || character > '9') {
            return false;
        }
        const std::size_t digit = static_cast<std::size_t>(character - '0');
        if (parsed > (static_cast<std::size_t>(-1) - digit) / 10) {
            return false;
        }
        parsed = parsed * 10 + digit;
    }
    result = parsed;
    return true;
}

}  // namespace

SettingsStore::SettingsStore(std::filesystem::path path) : path_(std::move(path)) {}

SettingsLoadResult SettingsStore::load() const {
    SettingsLoadResult result;
    std::ifstream input(path_);
    if (!input.is_open()) {
        result.status =
            std::filesystem::exists(path_) ? SettingsStatus::io_error : SettingsStatus::ok;
        return result;
    }

    EngineSettings settings;
    std::string line;
    int version = 0;
    bool has_version = false;
    bool has_layout = false;
    bool has_candidate_limit = false;
    bool has_learning = false;

    while (std::getline(input, line)) {
        if (line.empty()) {
            continue;
        }
        const std::size_t separator = line.find('=');
        if (separator == std::string::npos) {
            result.status = SettingsStatus::invalid_format;
            result.error = "Malformed settings line";
            return result;
        }
        const std::string key = line.substr(0, separator);
        const std::string value = line.substr(separator + 1);
        if (key == "version") {
            std::size_t parsed = 0;
            if (!parse_uint(value, parsed)) {
                result.status = SettingsStatus::invalid_format;
                result.error = "Invalid settings version";
                return result;
            }
            version = static_cast<int>(parsed);
            has_version = true;
        } else if (key == "active_layout") {
            if (value.empty()) {
                result.status = SettingsStatus::invalid_format;
                result.error = "active_layout cannot be empty";
                return result;
            }
            settings.active_layout_id = value;
            has_layout = true;
        } else if (key == "candidate_limit") {
            if (!parse_uint(value, settings.candidate_limit) || settings.candidate_limit == 0 ||
                settings.candidate_limit > 100) {
                result.status = SettingsStatus::invalid_format;
                result.error = "Invalid candidate_limit";
                return result;
            }
            has_candidate_limit = true;
        } else if (key == "learning_enabled") {
            if (value == "true") {
                settings.learning_enabled = true;
            } else if (value == "false") {
                settings.learning_enabled = false;
            } else {
                result.status = SettingsStatus::invalid_format;
                result.error = "Invalid learning_enabled";
                return result;
            }
            has_learning = true;
        } else {
            result.status = SettingsStatus::invalid_format;
            result.error = "Unknown settings key";
            return result;
        }
    }

    if (!has_version) {
        result.status = SettingsStatus::invalid_format;
        result.error = "Missing settings version";
        return result;
    }
    if (version != kSettingsVersion) {
        result.status = SettingsStatus::unsupported_version;
        result.error = "Unsupported settings version";
        return result;
    }
    if (!has_layout || !has_candidate_limit || !has_learning) {
        result.status = SettingsStatus::invalid_format;
        result.error = "Missing required settings";
        return result;
    }

    result.settings = std::move(settings);
    return result;
}

SettingsStatus SettingsStore::save(const EngineSettings& settings) const {
    if (settings.active_layout_id.empty() || settings.candidate_limit == 0 ||
        settings.candidate_limit > 100) {
        return SettingsStatus::invalid_format;
    }

    std::error_code error;
    const std::filesystem::path parent = path_.parent_path();
    if (!parent.empty()) {
        std::filesystem::create_directories(parent, error);
        if (error) {
            return SettingsStatus::io_error;
        }
    }

    const std::filesystem::path temporary = path_;
    std::filesystem::path temporary_path = temporary;
    temporary_path += ".tmp";

    std::ofstream output(temporary_path, std::ios::trunc);
    if (!output.is_open()) {
        return SettingsStatus::io_error;
    }
    output << "version=" << kSettingsVersion << '\n'
           << "active_layout=" << settings.active_layout_id << '\n'
           << "candidate_limit=" << settings.candidate_limit << '\n'
           << "learning_enabled=" << (settings.learning_enabled ? "true" : "false") << '\n';
    output.close();
    if (!output) {
        std::filesystem::remove(temporary_path, error);
        return SettingsStatus::io_error;
    }

    std::filesystem::rename(temporary_path, path_, error);
    if (error) {
        std::filesystem::remove(temporary_path, error);
        return SettingsStatus::io_error;
    }
    return SettingsStatus::ok;
}

}  // namespace kalaime::settings
