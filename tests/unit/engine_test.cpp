#include "suborno/core/engine.hpp"

#include <gtest/gtest.h>
#include <sqlite3.h>

#include <cstdio>
#include <filesystem>
#include <fstream>

#include "suborno/candidates/candidate_engine.hpp"
#include "suborno/candidates/language_intelligence.hpp"
#include "suborno/dictionary/dictionary.hpp"
#include "suborno/suborno.h"
#include "suborno/layout-api/declarative_layout.hpp"
#include "suborno/layout-api/phonetic_layout.hpp"
#include "suborno/layouts/phonetic/suborno_phonetic_layout.hpp"
#include "suborno/settings/settings.hpp"

namespace {

class TestLayout final : public suborno::layout::PhoneticLayout {
   public:
    std::string transliterate(const std::string& input) const override { return "test:" + input; }

    std::vector<suborno::layout::Candidate> generate_candidates(
        const std::string& input) const override {
        return {{transliterate(input), transliterate(input), "layout:test-layout",
                 suborno::layout::CandidateSource::layout, 1.0, "test layout"}};
    }
};

}  // namespace

static_assert(!std::is_copy_constructible_v<suborno::dictionary::Dictionary>);
static_assert(!std::is_copy_assignable_v<suborno::dictionary::Dictionary>);

TEST(EngineTest, RegistersDefaultPhoneticLayout) {
    suborno::core::Engine engine;
    EXPECT_EQ(engine.active_layout_id(), "suborno-phonetic");
    const auto* metadata = engine.layout_metadata("suborno-phonetic");
    ASSERT_NE(metadata, nullptr);
    EXPECT_EQ(metadata->display_name, "SUBORNO Phonetic");

    auto session = engine.create_session();
    ASSERT_NE(session, nullptr);
    ASSERT_EQ(session->append_utf8("ami"), suborno::layouts::InputStatus::updated);
    EXPECT_EQ(session->snapshot().preedit_utf8, "আমি");
}

TEST(EngineTest, RegistersAndSwitchesLayouts) {
    suborno::core::Engine engine;
    EXPECT_TRUE(engine.register_layout("test", std::make_unique<TestLayout>()));
    EXPECT_TRUE(engine.set_active_layout("test"));
    EXPECT_EQ(engine.active_layout_id(), "test");

    auto session = engine.create_session();
    ASSERT_NE(session, nullptr);
    ASSERT_EQ(session->append_utf8("x"), suborno::layouts::InputStatus::updated);
    EXPECT_EQ(session->snapshot().preedit_utf8, "test:x");
}

TEST(EngineTest, CandidateEngineAddsDictionaryExactMatches) {
    const std::string db_path = "/tmp/suborno_candidate_engine_test.sqlite";
    std::remove(db_path.c_str());
    suborno::dictionary::Dictionary dictionary(db_path);
    dictionary.add_entry("আমি", "আমি", 10);

    suborno::layouts::SubornoPhoneticLayout layout;
    suborno::candidates::CandidateEngine engine(layout, dictionary);
    const auto candidates = engine.make_candidates("ami");

    ASSERT_FALSE(candidates.empty());
    EXPECT_EQ(candidates.front().text, "আমি");
    EXPECT_EQ(candidates.front().normalized_text, "আমি");
    EXPECT_FALSE(candidates.front().stable_id.empty());
    EXPECT_EQ(candidates.front().source, suborno::layout::CandidateSource::system_dictionary);
}

TEST(EngineTest, DictionarySchemaCommitAndReset) {
    const std::string db_path = "/tmp/suborno_dictionary_learning_test.sqlite";
    std::remove(db_path.c_str());
    suborno::dictionary::Dictionary dictionary(db_path);

    EXPECT_EQ(dictionary.schema_version(), 1);
    dictionary.add_entry("বাংলা", "bangla", 5);
    dictionary.record_commit("বাংলা", "bangla");

    const auto matches = dictionary.find_matches("bangla");
    ASSERT_FALSE(matches.empty());
    EXPECT_EQ(matches.front(), "বাংলা");

    dictionary.clear_user_entries();
    EXPECT_EQ(dictionary.find_matches("bangla").front(), "বাংলা");
}

TEST(EngineTest, DictionaryMigratesLegacySchema) {
    const std::string db_path = "/tmp/suborno_dictionary_migration_test.sqlite";
    std::remove(db_path.c_str());

    sqlite3* db = nullptr;
    ASSERT_EQ(sqlite3_open(db_path.c_str(), &db), SQLITE_OK);
    ASSERT_EQ(sqlite3_exec(db,
                           "CREATE TABLE dictionary_entries("
                           "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                           "word TEXT NOT NULL UNIQUE,"
                           "normalized TEXT NOT NULL,"
                           "frequency INTEGER NOT NULL DEFAULT 1,"
                           "created_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP);"
                           "INSERT INTO dictionary_entries(word, normalized, frequency)"
                           " VALUES('বাংলা', 'bangla', 5);",
                           nullptr, nullptr, nullptr),
              SQLITE_OK);
    sqlite3_close(db);

    suborno::dictionary::Dictionary dictionary(db_path);
    EXPECT_EQ(dictionary.schema_version(), 1);
    EXPECT_EQ(dictionary.find_matches("bangla").front(), "বাংলা");
    dictionary.record_commit("বাংলা", "bangla");
    EXPECT_EQ(dictionary.find_matches("bangla").front(), "বাংলা");
    std::remove(db_path.c_str());
}

TEST(EngineTest, DictionaryRejectsCorruptDatabase) {
    const std::string db_path = "/tmp/suborno_dictionary_corrupt_test.sqlite";
    std::remove(db_path.c_str());
    {
        std::ofstream output(db_path, std::ios::binary);
        output << "not a sqlite database";
    }

    EXPECT_THROW(suborno::dictionary::Dictionary dictionary(db_path),
                 suborno::dictionary::DictionaryError);
    std::remove(db_path.c_str());
}

TEST(EngineTest, DictionaryPrefixLookupAndReset) {
    const std::string db_path = "/tmp/suborno_dictionary_prefix_test.sqlite";
    std::remove(db_path.c_str());
    suborno::dictionary::Dictionary dictionary(db_path);
    dictionary.add_entry("বাংলা", "bangla", 5);
    dictionary.add_entry("বাঙালি", "bangali", 3);

    const auto matches = dictionary.words_for_prefix("বা");
    ASSERT_FALSE(matches.empty());
    dictionary.clear_user_entries();

    std::remove(db_path.c_str());
}

TEST(EngineTest, EngineConfigPersistsSettingsAndOwnsDictionary) {
    const std::filesystem::path settings_path =
        std::filesystem::temp_directory_path() / "suborno_engine_settings.conf";
    const std::filesystem::path dictionary_path =
        std::filesystem::temp_directory_path() / "suborno_engine_dictionary.sqlite";
    std::filesystem::remove(settings_path);
    std::filesystem::remove(dictionary_path);

    suborno::core::EngineConfig config;
    config.settings_path = settings_path;
    config.dictionary_path = dictionary_path;
    suborno::core::Engine engine(config);

    suborno::settings::EngineSettings settings = engine.settings();
    settings.candidate_limit = 24;
    settings.learning_enabled = false;
    EXPECT_EQ(engine.save_settings(settings), suborno::settings::SettingsStatus::ok);
    EXPECT_EQ(engine.settings().candidate_limit, 24U);
    ASSERT_NE(engine.dictionary(), nullptr);

    engine.dictionary()->add_entry("আমি", "ami", 3);
    EXPECT_FALSE(engine.dictionary()->find_matches("ami").empty());

    std::filesystem::remove(settings_path);
    std::filesystem::remove(dictionary_path);
}

TEST(EngineTest, LoadsDeclarativeLayoutMapping) {
    const std::filesystem::path mapping =
        std::filesystem::temp_directory_path() / "suborno_layout_test.tsv";
    std::ofstream output(mapping, std::ios::trunc);
    output << "# test mapping\n";
    output << "a\tআ\n";
    output << "k\tক\n";
    output.close();

    suborno::layout::DeclarativeLayout layout("test-layout", mapping);
    ASSERT_TRUE(layout.load());
    EXPECT_EQ(layout.transliterate("ka"), "কআ");
    const auto candidates = layout.generate_candidates("ka");
    ASSERT_EQ(candidates.size(), 1U);
    EXPECT_EQ(candidates.front().stable_id, "layout:test-layout:কআ");

    std::filesystem::remove(mapping);
}

TEST(EngineTest, DeterministicLanguageIntelligence) {
    const std::string db_path = "/tmp/suborno_language_intelligence_test.sqlite";
    std::remove(db_path.c_str());
    suborno::dictionary::Dictionary dictionary(db_path);
    dictionary.add_entry("বাংলা", "bangla", 5);
    suborno::candidates::LanguageIntelligence intelligence(dictionary);

    EXPECT_TRUE(intelligence.is_known_word("bangla"));
    EXPECT_EQ(intelligence.correct_or_original("bangla"), "বাংলা");
    EXPECT_EQ(intelligence.correct_or_original("unknown"), "unknown");
    EXPECT_FALSE(intelligence.suggest_prefix("বা").empty());
    std::remove(db_path.c_str());
}

TEST(EngineTest, CApiCreatesAndCommitsSession) {
    SUBORNO_Engine* engine = suborno_engine_create();
    ASSERT_NE(engine, nullptr);
    SUBORNO_Session* session = suborno_session_create(engine);
    ASSERT_NE(session, nullptr);

    EXPECT_GE(suborno_session_append_utf8(session, "ami"), 0);
    char buffer[256];
    EXPECT_GE(suborno_session_snapshot(session, buffer, sizeof(buffer)), 0);
    EXPECT_STREQ(buffer, "আমি");

    char* committed = nullptr;
    std::size_t committed_size = 0;
    EXPECT_GE(suborno_session_commit(session, &committed, &committed_size), 0);
    ASSERT_NE(committed, nullptr);
    EXPECT_EQ(std::string(committed), "আমি");
    suborno_string_free(committed);

    suborno_session_destroy(session);
    suborno_engine_destroy(engine);
}
