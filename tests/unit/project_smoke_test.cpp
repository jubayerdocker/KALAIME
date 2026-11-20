#include <gtest/gtest.h>

#include "suborno/core/project.hpp"
#include "suborno/layouts/phonetic/suborno_phonetic_layout.hpp"

TEST(ProjectSmokeTest, ExposesInitialInfrastructureAbiVersion) {
    EXPECT_EQ(suborno::project_abi_version(), 1U);
}

TEST(ProjectSmokeTest, HandlesCoreBanglaTransliterationExamples) {
    const suborno::layouts::SubornoPhoneticLayout layout;

    EXPECT_EQ(layout.transliterate("ami"), "আমি");
    EXPECT_EQ(layout.transliterate("bangla"), "বাংলা");
    EXPECT_EQ(layout.transliterate("bangladesh"), "বাংলাদেশ");
    EXPECT_EQ(layout.transliterate("bhasha"), "ভাশা");
    EXPECT_EQ(layout.transliterate("shikkha"), "শিক্ষা");
    EXPECT_EQ(layout.transliterate("bhalobasha"), "ভালোবাশা");
}
