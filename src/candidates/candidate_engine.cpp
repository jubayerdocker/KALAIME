#include "kalaime/candidates/candidate_engine.hpp"

#include <unordered_set>

#include "kalaime/dictionary/dictionary.hpp"
#include "kalaime/unicode/bengali.hpp"

namespace kalaime::candidates {

CandidateEngine::CandidateEngine(const kalaime::layout::PhoneticLayout& layout) : layout_(layout) {}
CandidateEngine::CandidateEngine(const kalaime::layout::PhoneticLayout& layout,
                                 const kalaime::dictionary::Dictionary& dictionary)
    : layout_(layout), dictionary_(&dictionary) {}

std::vector<kalaime::layout::Candidate> CandidateEngine::make_candidates(
    const std::string& input) const {
    std::vector<kalaime::layout::Candidate> result;
    std::unordered_set<std::string> seen;

    const std::string normalized = kalaime::unicode::normalize_nfc(layout_.transliterate(input));

    if (dictionary_ != nullptr) {
        for (const std::string& word : dictionary_->find_matches(normalized)) {
            if (seen.insert(word).second) {
                result.push_back({word, 1.0, "exact dictionary match"});
            }
        }
        for (const std::string& word : dictionary_->find_matches(input)) {
            if (seen.insert(word).second) {
                result.push_back({word, 1.0, "phonetic dictionary match"});
            }
        }
    }

    for (const auto& candidate : layout_.generate_candidates(input)) {
        if (seen.insert(candidate.text).second) {
            result.push_back(candidate);
        }
    }

    for (std::size_t index = 0; index < result.size(); ++index) {
        if (result[index].rationale.find("dictionary") == std::string::npos) {
            result[index].score = 1.0 / static_cast<double>(index + 1);
        }
    }
    return result;
}

}  // namespace kalaime::candidates
