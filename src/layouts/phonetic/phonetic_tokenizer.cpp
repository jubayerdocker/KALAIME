#include "phonetic_tokenizer.hpp"

#include <array>

namespace suborno::layouts::phonetic::detail {
namespace {

using Kind = TokenKind;
using Definition = TokenDefinition;

constexpr std::array kTokens{
    Definition{"chh", Kind::consonant, "ছ", {}}, Definition{"ksh", Kind::cluster, "ক্ষ", {}},
    Definition{"kkh", Kind::cluster, "ক্ষ", {}},  Definition{"jny", Kind::cluster, "জ্ঞ", {}},
    Definition{"Th", Kind::consonant, "ঠ", {}},  Definition{"Dh", Kind::consonant, "ঢ", {}},
    Definition{"Rh", Kind::consonant, "ঢ়", {}},  Definition{"Ch", Kind::consonant, "ছ", {}},
    Definition{"Gh", Kind::consonant, "ঘ", {}},  Definition{"Jh", Kind::consonant, "ঝ", {}},
    Definition{"Kh", Kind::consonant, "খ", {}},  Definition{"Ph", Kind::consonant, "ফ", {}},
    Definition{"Bh", Kind::consonant, "ভ", {}},  Definition{"aa", Kind::vowel, "আ", "া"},
    Definition{"ii", Kind::vowel, "ঈ", "ী"},     Definition{"ee", Kind::vowel, "ঈ", "ী"},
    Definition{"uu", Kind::vowel, "ঊ", "ূ"},      Definition{"oo", Kind::vowel, "ঊ", "ূ"},
    Definition{"ri", Kind::vowel, "ঋ", "ৃ"},      Definition{"oi", Kind::vowel, "ঐ", "ৈ"},
    Definition{"ai", Kind::vowel, "ঐ", "ৈ"},     Definition{"ou", Kind::vowel, "ঔ", "ৌ"},
    Definition{"au", Kind::vowel, "ঔ", "ৌ"},     Definition{"kh", Kind::consonant, "খ", {}},
    Definition{"gh", Kind::consonant, "ঘ", {}},  Definition{"ch", Kind::consonant, "ছ", {}},
    Definition{"jh", Kind::consonant, "ঝ", {}},  Definition{"ny", Kind::consonant, "ঞ", {}},
    Definition{"th", Kind::consonant, "থ", {}},  Definition{"dh", Kind::consonant, "ধ", {}},
    Definition{"ph", Kind::consonant, "ফ", {}},  Definition{"bh", Kind::consonant, "ভ", {}},
    Definition{"ng", Kind::nasal_ng, "ং", "ঙ"},  Definition{"gy", Kind::cluster, "জ্ঞ", {}},
    Definition{"jn", Kind::cluster, "জ্ঞ", {}},   Definition{"rh", Kind::consonant, "র\u09CDহ", {}},
    Definition{"sh", Kind::consonant, "শ", {}},  Definition{"Sh", Kind::consonant, "ষ", {}},
    Definition{"a", Kind::vowel, "আ", "া"},      Definition{"ô", Kind::vowel, "অ", ""},
    Definition{"O", Kind::vowel, "অ", ""},       Definition{"i", Kind::vowel, "ই", "ি"},
    Definition{"u", Kind::vowel, "উ", "ু"},       Definition{"e", Kind::vowel, "এ", "ে"},
    Definition{"o", Kind::vowel, "ও", "ো"},      Definition{"w", Kind::vowel, "ও", "ো"},
    Definition{"W", Kind::vowel, "ও", "ো"},      Definition{"x", Kind::cluster, "ক্ষ", {}},
    Definition{"k", Kind::consonant, "ক", {}},   Definition{"q", Kind::consonant, "ক", {}},
    Definition{"g", Kind::consonant, "গ", {}},   Definition{"c", Kind::consonant, "চ", {}},
    Definition{"j", Kind::consonant, "জ", {}},   Definition{"z", Kind::consonant, "জ", {}},
    Definition{"T", Kind::consonant, "ট", {}},   Definition{"D", Kind::consonant, "ড", {}},
    Definition{"N", Kind::consonant, "ণ", {}},   Definition{"t", Kind::consonant, "ত", {}},
    Definition{"d", Kind::consonant, "দ", {}},   Definition{"n", Kind::nasal_n, "ন", "ং"},
    Definition{"p", Kind::consonant, "প", {}},   Definition{"f", Kind::consonant, "ফ", {}},
    Definition{"b", Kind::consonant, "ব", {}},   Definition{"v", Kind::consonant, "ভ", {}},
    Definition{"m", Kind::consonant, "ম", {}},   Definition{"y", Kind::consonant, "য", {}},
    Definition{"Y", Kind::consonant, "য়", {}},   Definition{"r", Kind::consonant, "র", {}},
    Definition{"R", Kind::consonant, "ড়", {}},   Definition{"l", Kind::consonant, "ল", {}},
    Definition{"s", Kind::consonant, "স", {}},   Definition{"h", Kind::consonant, "হ", {}},
    Definition{"M", Kind::anusvara, "ং", {}},    Definition{"H", Kind::visarga, "ঃ", {}},
    Definition{"A", Kind::vowel, "আ", "া"},      Definition{"B", Kind::consonant, "ব", {}},
    Definition{"C", Kind::consonant, "চ", {}},   Definition{"E", Kind::vowel, "এ", "ে"},
    Definition{"F", Kind::consonant, "ফ", {}},   Definition{"G", Kind::consonant, "গ", {}},
    Definition{"I", Kind::vowel, "ই", "ি"},      Definition{"J", Kind::consonant, "জ", {}},
    Definition{"K", Kind::consonant, "ক", {}},   Definition{"L", Kind::consonant, "ল", {}},
    Definition{"P", Kind::consonant, "প", {}},   Definition{"Q", Kind::consonant, "ক", {}},
    Definition{"S", Kind::consonant, "স", {}},   Definition{"U", Kind::vowel, "উ", "ু"},
    Definition{"V", Kind::consonant, "ভ", {}},   Definition{"X", Kind::cluster, "ক্ষ", {}},
    Definition{"Z", Kind::consonant, "জ", {}},
};

}  // namespace

std::span<const TokenDefinition> token_registry() noexcept { return kTokens; }

std::optional<TokenMatch> match_token(std::string_view input, std::size_t offset) noexcept {
    if (offset >= input.size()) {
        return std::nullopt;
    }

    const TokenDefinition* best_match = nullptr;
    for (const TokenDefinition& token : kTokens) {
        if (token.input.size() <= input.size() - offset &&
            input.compare(offset, token.input.size(), token.input) == 0 &&
            (best_match == nullptr || token.input.size() > best_match->input.size())) {
            best_match = &token;
        }
    }

    if (best_match == nullptr) {
        return std::nullopt;
    }
    return TokenMatch{best_match, best_match->input.size()};
}

}  // namespace suborno::layouts::phonetic::detail
