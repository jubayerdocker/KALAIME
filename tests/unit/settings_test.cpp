#include "kalaime/settings/settings.hpp"

#include <gtest/gtest.h>

#include <cstdio>
#include <filesystem>
#include <fstream>

TEST(SettingsTest, SavesAndLoadsVersionedSettings) {
    const std::filesystem::path path =
        std::filesystem::temp_directory_path() / "kalaime_settings_test.conf";
    std::filesystem::remove(path);

    kalaime::settings::SettingsStore store(path);
    kalaime::settings::EngineSettings settings;
    settings.active_layout_id = "kala-phonetic";
    settings.candidate_limit = 24;
    settings.learning_enabled = false;

    EXPECT_EQ(store.save(settings), kalaime::settings::SettingsStatus::ok);
    const auto loaded = store.load();
    EXPECT_EQ(loaded.status, kalaime::settings::SettingsStatus::ok);
    EXPECT_EQ(loaded.settings.active_layout_id, settings.active_layout_id);
    EXPECT_EQ(loaded.settings.candidate_limit, settings.candidate_limit);
    EXPECT_FALSE(loaded.settings.learning_enabled);

    std::filesystem::remove(path);
}

TEST(SettingsTest, MissingFileUsesDefaults) {
    const std::filesystem::path path =
        std::filesystem::temp_directory_path() / "kalaime_settings_missing_test.conf";
    std::filesystem::remove(path);

    const auto loaded = kalaime::settings::SettingsStore(path).load();
    EXPECT_EQ(loaded.status, kalaime::settings::SettingsStatus::ok);
    EXPECT_EQ(loaded.settings.active_layout_id, "kala-phonetic");
    EXPECT_EQ(loaded.settings.candidate_limit, 16U);
    EXPECT_TRUE(loaded.settings.learning_enabled);
}

TEST(SettingsTest, RejectsMalformedOrUnsupportedFiles) {
    const std::filesystem::path malformed =
        std::filesystem::temp_directory_path() / "kalaime_settings_malformed_test.conf";
    std::ofstream malformed_output(malformed, std::ios::trunc);
    malformed_output << "not-a-settings-line\n";
    malformed_output.close();

    const auto malformed_result = kalaime::settings::SettingsStore(malformed).load();
    EXPECT_EQ(malformed_result.status, kalaime::settings::SettingsStatus::invalid_format);

    const std::filesystem::path unsupported =
        std::filesystem::temp_directory_path() / "kalaime_settings_unsupported_test.conf";
    std::ofstream unsupported_output(unsupported, std::ios::trunc);
    unsupported_output << "version=99\n"
                       << "active_layout=kala-phonetic\n"
                       << "candidate_limit=16\n"
                       << "learning_enabled=true\n";
    unsupported_output.close();

    const auto unsupported_result = kalaime::settings::SettingsStore(unsupported).load();
    EXPECT_EQ(unsupported_result.status, kalaime::settings::SettingsStatus::unsupported_version);

    std::filesystem::remove(malformed);
    std::filesystem::remove(unsupported);
}
