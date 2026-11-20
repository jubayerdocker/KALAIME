#include <gtest/gtest.h>
#include <sqlite3.h>
#include <unicode/uversion.h>

#include "suborno/core/project.hpp"
#include "suborno/dictionary/dictionary.hpp"
#include "suborno/layouts/phonetic/suborno_phonetic_layout.hpp"
#include "suborno/unicode/bengali.hpp"

TEST(TargetGraphSmokeTest, LinksPortableCoreAndProductionDependencies) {
    EXPECT_NE(suborno::project_abi_version(), 0U);
    EXPECT_GT(sqlite3_libversion_number(), 0);

    UVersionInfo icu_version{};
    u_getVersion(icu_version);
    EXPECT_GT(icu_version[0], 0);

    const auto inventory = suborno::unicode::supported_bengali_inventory();
    EXPECT_FALSE(inventory.empty());
    EXPECT_TRUE(suborno::unicode::is_bengali_codepoint(0x0985));
}

TEST(TargetGraphSmokeTest, DictionaryAndLayoutManageRealInputWorkflow) {
    constexpr int kAmiFrequency = 3;
    constexpr int kBanglaFrequency = 5;

    const std::string db_path = "/tmp/suborno_dictionary_test.sqlite";
    suborno::dictionary::Dictionary dictionary(db_path);
    dictionary.add_entry("আমি", "ami", kAmiFrequency);
    dictionary.add_entry("বাংলা", "bangla", kBanglaFrequency);

    const auto matches = dictionary.find_matches("ami");
    EXPECT_FALSE(matches.empty());
    EXPECT_EQ(matches.front(), "আমি");

    const suborno::layouts::SubornoPhoneticLayout layout;
    EXPECT_EQ(layout.transliterate("ami"), "আমি");
    EXPECT_EQ(layout.transliterate("bangla"), "বাংলা");
}
