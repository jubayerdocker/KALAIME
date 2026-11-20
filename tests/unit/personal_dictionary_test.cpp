#include <gtest/gtest.h>

#include <string>

#include "suborno/dictionary/dictionary.hpp"

TEST(PersonalDictionaryTest, AddAndListUserWords) {
    const std::string db_path = "/tmp/suborno_personal_add_test.sqlite";
    std::remove(db_path.c_str());

    suborno::dictionary::Dictionary dictionary(db_path);

    dictionary.add_user_word("সুইডেন", "sweden");
    dictionary.add_user_word("নরওয়ে", "norway");

    const auto user_words = dictionary.list_user_words();
    ASSERT_EQ(user_words.size(), 2U);

    std::remove(db_path.c_str());
}

TEST(PersonalDictionaryTest, GetUserFrequency) {
    const std::string db_path = "/tmp/suborno_personal_freq_test.sqlite";
    std::remove(db_path.c_str());

    suborno::dictionary::Dictionary dictionary(db_path);

    EXPECT_EQ(dictionary.get_user_frequency("সুইডেন"), 0);

    dictionary.add_user_word("সুইডেন", "sweden");
    EXPECT_EQ(dictionary.get_user_frequency("সুইডেন"), 1);

    dictionary.add_user_word("সুইডেন", "sweden");
    EXPECT_EQ(dictionary.get_user_frequency("সুইডেন"), 2);

    std::remove(db_path.c_str());
}

TEST(PersonalDictionaryTest, RemoveUserWord) {
    const std::string db_path = "/tmp/suborno_personal_remove_test.sqlite";
    std::remove(db_path.c_str());

    suborno::dictionary::Dictionary dictionary(db_path);

    dictionary.add_user_word("সুইডেন", "sweden");
    EXPECT_EQ(dictionary.get_user_frequency("সুইডেন"), 1);

    dictionary.remove_user_word("সুইডেন");
    EXPECT_EQ(dictionary.get_user_frequency("সুইডেন"), 0);

    const auto user_words = dictionary.list_user_words();
    EXPECT_TRUE(user_words.empty());

    std::remove(db_path.c_str());
}

TEST(PersonalDictionaryTest, UserWordsRankedInFindMatches) {
    const std::string db_path = "/tmp/suborno_personal_rank_test.sqlite";
    std::remove(db_path.c_str());

    suborno::dictionary::Dictionary dictionary(db_path);

    dictionary.add_entry("বাংলা", "bangla", 5);
    dictionary.add_user_word("বাংলাদেশ", "bangladesh");

    const auto matches = dictionary.find_matches("bangla");
    ASSERT_FALSE(matches.empty());

    std::remove(db_path.c_str());
}
