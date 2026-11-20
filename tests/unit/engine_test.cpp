#include "kalaime/core/engine.hpp"

#include <gtest/gtest.h>
#include <sqlite3.h>

#include <cstdio>
#include <fstream>

#include "kalaime/candidates/candidate_engine.hpp"
#include "kalaime/dictionary/dictionary.hpp"
#include "kalaime/layout-api/phonetic_layout.hpp"
#include "kalaime/layouts/phonetic/kala_phonetic_layout.hpp"

namespace {

class TestLayout final : public kalaime::layout::PhoneticLayout {
   public:
    std::string transliterate(const std::string& input) const override { return "test:" + input; }

    std::vector<kalaime::layout::Candidate> generate_candidates(
        const std::string& input) const override {
        return {{transliterate(input), 1.0, "test layout"}};
    }
};

}  // namespace

static_assert(!std::is_copy_constructible_v<kalaime::dictionary::Dictionary>);
static_assert(!std::is_copy_assignable_v<kalaime::dictionary::Dictionary>);

TEST(EngineTest, RegistersDefaultPhoneticLayout) {
    kalaime::core::Engine engine;
    EXPECT_EQ(engine.active_layout_id(), "kala-phonetic");

    auto session = engine.create_session();
    ASSERT_NE(session, nullptr);
    ASSERT_EQ(session->append_utf8("ami"), kalaime::layouts::InputStatus::updated);
    EXPECT_EQ(session->snapshot().preedit_utf8, "আমি");
}

TEST(EngineTest, RegistersAndSwitchesLayouts) {
    kalaime::core::Engine engine;
    EXPECT_TRUE(engine.register_layout("test", std::make_unique<TestLayout>()));
    EXPECT_TRUE(engine.set_active_layout("test"));
    EXPECT_EQ(engine.active_layout_id(), "test");

    auto session = engine.create_session();
    ASSERT_NE(session, nullptr);
    ASSERT_EQ(session->append_utf8("x"), kalaime::layouts::InputStatus::updated);
    EXPECT_EQ(session->snapshot().preedit_utf8, "test:x");
}

TEST(EngineTest, CandidateEngineAddsDictionaryExactMatches) {
    const std::string db_path = "/tmp/kalaime_candidate_engine_test.sqlite";
    std::remove(db_path.c_str());
    kalaime::dictionary::Dictionary dictionary(db_path);
    dictionary.add_entry("আমি", "আমি", 10);

    kalaime::layouts::KalaPhoneticLayout layout;
    kalaime::candidates::CandidateEngine engine(layout, dictionary);
    const auto candidates = engine.make_candidates("ami");

    ASSERT_FALSE(candidates.empty());
    EXPECT_EQ(candidates.front().text, "আমি");
}

TEST(EngineTest, DictionarySchemaCommitAndReset) {
    const std::string db_path = "/tmp/kalaime_dictionary_learning_test.sqlite";
    std::remove(db_path.c_str());
    kalaime::dictionary::Dictionary dictionary(db_path);

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
    const std::string db_path = "/tmp/kalaime_dictionary_migration_test.sqlite";
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

    kalaime::dictionary::Dictionary dictionary(db_path);
    EXPECT_EQ(dictionary.schema_version(), 1);
    EXPECT_EQ(dictionary.find_matches("bangla").front(), "বাংলা");
    dictionary.record_commit("বাংলা", "bangla");
    EXPECT_EQ(dictionary.find_matches("bangla").front(), "বাংলা");
    std::remove(db_path.c_str());
}

TEST(EngineTest, DictionaryRejectsCorruptDatabase) {
    const std::string db_path = "/tmp/kalaime_dictionary_corrupt_test.sqlite";
    std::remove(db_path.c_str());
    {
        std::ofstream output(db_path, std::ios::binary);
        output << "not a sqlite database";
    }

    EXPECT_THROW(kalaime::dictionary::Dictionary dictionary(db_path),
                 kalaime::dictionary::DictionaryError);
    std::remove(db_path.c_str());
}

TEST(EngineTest, DictionaryPrefixLookupAndReset) {
    const std::string db_path = "/tmp/kalaime_dictionary_prefix_test.sqlite";
    std::remove(db_path.c_str());
    kalaime::dictionary::Dictionary dictionary(db_path);
    dictionary.add_entry("বাংলা", "bangla", 5);
    dictionary.add_entry("বাঙালি", "bangali", 3);

    const auto matches = dictionary.words_for_prefix("বা");
    ASSERT_FALSE(matches.empty());
    dictionary.clear_user_entries();

    std::remove(db_path.c_str());
}
