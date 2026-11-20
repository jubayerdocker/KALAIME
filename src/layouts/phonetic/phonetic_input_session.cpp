#include "suborno/layouts/phonetic/phonetic_input_session.hpp"

#include <unordered_set>
#include <utility>

#include "suborno/dictionary/dictionary.hpp"
#include "suborno/layouts/phonetic/suborno_phonetic_layout.hpp"
#include "suborno/unicode/bengali.hpp"
namespace {
std::vector<suborno::layout::Candidate> make_session_candidates(
    const suborno::layout::PhoneticLayout& layout, const std::string& input,
    const suborno::dictionary::Dictionary* dictionary) {
    std::vector<suborno::layout::Candidate> candidates;
    std::unordered_set<std::string> seen;
    if (dictionary != nullptr) {
        const std::string normalized = suborno::unicode::normalize_nfc(layout.transliterate(input));
        for (const std::string& word : dictionary->find_matches(normalized)) {
            if (seen.insert(word).second) {
                candidates.push_back({word, suborno::unicode::normalize_nfc(word),
                                      "system-dictionary:" + suborno::unicode::normalize_nfc(word),
                                      suborno::layout::CandidateSource::system_dictionary, 1.0,
                                      "exact dictionary match"});
            }
        }
    }
    for (const auto& candidate : layout.generate_candidates(input)) {
        if (seen.insert(candidate.text).second) {
            candidates.push_back(candidate);
        }
    }
    for (std::size_t index = 0; index < candidates.size(); ++index) {
        if (candidates[index].source == suborno::layout::CandidateSource::layout) {
            candidates[index].score = 1.0 / static_cast<double>(index + 1);
        }
    }
    return candidates;
}
}  // namespace
#include "suborno/unicode/bengali.hpp"
#include "phonetic_parser.hpp"

namespace suborno::layouts {

PhoneticInputSession::PhoneticInputSession() {
    static const SubornoPhoneticLayout default_layout;
    layout_ = &default_layout;
}

PhoneticInputSession::PhoneticInputSession(const suborno::layout::PhoneticLayout& layout)
    : layout_(&layout) {}

PhoneticInputSession::PhoneticInputSession(const suborno::layout::PhoneticLayout& layout,
                                           suborno::dictionary::Dictionary& dictionary)
    : layout_(&layout), dictionary_(&dictionary) {}

InputStatus PhoneticInputSession::append_utf8(std::string_view input) {
    if (!suborno::unicode::is_valid_utf8(input)) {
        return InputStatus::invalid_utf8;
    }
    if (input.empty()) {
        return InputStatus::empty;
    }

    std::string updated_input = raw_input_;
    updated_input.append(input);
    if (suborno::unicode::count_utf8_code_points(updated_input) > kMaxInputCodePoints) {
        return InputStatus::input_limit;
    }

    const auto parsed = suborno::layouts::phonetic::detail::parse_primary(updated_input, false);
    raw_input_ = std::move(updated_input);
    selected_candidate_index_ = 0;
    if (parsed.pending) {
        return InputStatus::pending;
    }
    return parsed.literal_passthrough ? InputStatus::literal_passthrough : InputStatus::updated;
}

InputStatus PhoneticInputSession::backspace() {
    if (raw_input_.empty()) {
        return InputStatus::empty;
    }

    const auto graphemes = suborno::unicode::split_graphemes(raw_input_);
    std::string updated_input = raw_input_;
    updated_input.resize(updated_input.size() - graphemes.back().size());
    const auto parsed = suborno::layouts::phonetic::detail::parse_primary(updated_input, false);
    raw_input_ = std::move(updated_input);
    selected_candidate_index_ = 0;
    if (raw_input_.empty()) {
        return InputStatus::empty;
    }
    if (parsed.pending) {
        return InputStatus::pending;
    }
    return parsed.literal_passthrough ? InputStatus::literal_passthrough : InputStatus::updated;
}

CommitResult PhoneticInputSession::commit() {
    if (raw_input_.empty()) {
        return {{}, InputStatus::empty};
    }

    const auto parsed = suborno::layouts::phonetic::detail::parse_primary(raw_input_, true);
    const std::string committed =
        parsed.literal_passthrough ? parsed.text : layout_->transliterate(raw_input_);
    if (dictionary_ != nullptr) {
        dictionary_->record_commit(committed, committed);
    }
    raw_input_.clear();
    selected_candidate_index_ = 0;
    return {committed,
            parsed.literal_passthrough ? InputStatus::literal_passthrough : InputStatus::updated};
}

CommitResult PhoneticInputSession::commit_candidate(std::size_t index) {
    if (raw_input_.empty()) {
        return {{}, InputStatus::empty};
    }

    std::vector<suborno::layout::Candidate> candidates;
    if (dictionary_ != nullptr) {
        candidates = make_session_candidates(*layout_, raw_input_, dictionary_);
    } else {
        candidates = layout_->generate_candidates(raw_input_);
    }
    if (index >= candidates.size()) {
        return {{}, InputStatus::candidate_out_of_range};
    }
    selected_candidate_index_ = index;

    const auto parsed = suborno::layouts::phonetic::detail::parse_primary(raw_input_, true);
    const std::string committed = candidates[index].text;
    if (dictionary_ != nullptr) {
        dictionary_->record_commit(committed, candidates[index].normalized_text.empty()
                                                  ? committed
                                                  : candidates[index].normalized_text);
    }
    raw_input_.clear();
    selected_candidate_index_ = 0;
    return {committed,
            parsed.literal_passthrough ? InputStatus::literal_passthrough : InputStatus::updated};
}

bool PhoneticInputSession::select_candidate(std::size_t index) noexcept {
    const std::size_t candidate_count = snapshot().candidates.size();
    if (index >= candidate_count) {
        return false;
    }
    selected_candidate_index_ = index;
    return true;
}

bool PhoneticInputSession::select_next_candidate() noexcept {
    const std::size_t candidate_count = snapshot().candidates.size();
    if (candidate_count == 0) {
        return false;
    }
    if (selected_candidate_index_ + 1 >= candidate_count) {
        return false;
    }
    ++selected_candidate_index_;
    return true;
}

bool PhoneticInputSession::select_previous_candidate() noexcept {
    if (selected_candidate_index_ == 0) {
        return false;
    }
    --selected_candidate_index_;
    return true;
}

std::size_t PhoneticInputSession::selected_candidate_index() const noexcept {
    return selected_candidate_index_;
}

void PhoneticInputSession::reset() noexcept {
    raw_input_.clear();
    selected_candidate_index_ = 0;
}

CompositionSnapshot PhoneticInputSession::snapshot() const {
    CompositionSnapshot result;
    result.raw_input_utf8 = raw_input_;
    if (raw_input_.empty()) {
        result.status = InputStatus::empty;
        return result;
    }

    const auto parsed = suborno::layouts::phonetic::detail::parse_primary(raw_input_, false);
    result.preedit_utf8 =
        parsed.pending
            ? parsed.text
            : (parsed.literal_passthrough ? parsed.text : layout_->transliterate(raw_input_));
    if (parsed.pending) {
        result.status = InputStatus::pending;
    } else if (parsed.literal_passthrough) {
        result.status = InputStatus::literal_passthrough;
    } else {
        result.status = InputStatus::updated;
    }
    if (!parsed.pending) {
        if (dictionary_ != nullptr) {
            result.candidates = make_session_candidates(*layout_, raw_input_, dictionary_);
        } else {
            result.candidates = layout_->generate_candidates(raw_input_);
        }
        if (selected_candidate_index_ >= result.candidates.size()) {
            selected_candidate_index_ =
                result.candidates.empty() ? 0 : result.candidates.size() - 1;
        }
        result.selected_candidate_index = selected_candidate_index_;
    }
    return result;
}

}  // namespace suborno::layouts
