#include "kalaime/layouts/phonetic/phonetic_input_session.hpp"

#include <utility>

#include "kalaime/layouts/phonetic/kala_phonetic_layout.hpp"
#include "kalaime/unicode/bengali.hpp"
#include "phonetic_parser.hpp"

namespace kalaime::layouts {

PhoneticInputSession::PhoneticInputSession() {
    static const KalaPhoneticLayout default_layout;
    layout_ = &default_layout;
}

PhoneticInputSession::PhoneticInputSession(const kalaime::layout::PhoneticLayout& layout)
    : layout_(&layout) {}

InputStatus PhoneticInputSession::append_utf8(std::string_view input) {
    if (!kalaime::unicode::is_valid_utf8(input)) {
        return InputStatus::invalid_utf8;
    }
    if (input.empty()) {
        return InputStatus::empty;
    }

    std::string updated_input = raw_input_;
    updated_input.append(input);
    if (kalaime::unicode::count_utf8_code_points(updated_input) > kMaxInputCodePoints) {
        return InputStatus::input_limit;
    }

    const auto parsed = kalaime::layouts::phonetic::detail::parse_primary(updated_input, false);
    raw_input_ = std::move(updated_input);
    if (parsed.pending) {
        return InputStatus::pending;
    }
    return parsed.literal_passthrough ? InputStatus::literal_passthrough : InputStatus::updated;
}

InputStatus PhoneticInputSession::backspace() {
    if (raw_input_.empty()) {
        return InputStatus::empty;
    }

    const auto graphemes = kalaime::unicode::split_graphemes(raw_input_);
    std::string updated_input = raw_input_;
    updated_input.resize(updated_input.size() - graphemes.back().size());
    const auto parsed = kalaime::layouts::phonetic::detail::parse_primary(updated_input, false);
    raw_input_ = std::move(updated_input);
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

    const auto parsed = kalaime::layouts::phonetic::detail::parse_primary(raw_input_, true);
    const std::string committed =
        parsed.literal_passthrough ? parsed.text : layout_->transliterate(raw_input_);
    raw_input_.clear();
    return {committed,
            parsed.literal_passthrough ? InputStatus::literal_passthrough : InputStatus::updated};
}

CommitResult PhoneticInputSession::commit_candidate(std::size_t index) {
    if (raw_input_.empty()) {
        return {{}, InputStatus::empty};
    }

    const auto candidates = layout_->generate_candidates(raw_input_);
    if (index >= candidates.size()) {
        return {{}, InputStatus::candidate_out_of_range};
    }

    const auto parsed = kalaime::layouts::phonetic::detail::parse_primary(raw_input_, true);
    const std::string committed = candidates[index].text;
    raw_input_.clear();
    return {committed,
            parsed.literal_passthrough ? InputStatus::literal_passthrough : InputStatus::updated};
}

void PhoneticInputSession::reset() noexcept { raw_input_.clear(); }

CompositionSnapshot PhoneticInputSession::snapshot() const {
    CompositionSnapshot result;
    result.raw_input_utf8 = raw_input_;
    if (raw_input_.empty()) {
        result.status = InputStatus::empty;
        return result;
    }

    const auto parsed = kalaime::layouts::phonetic::detail::parse_primary(raw_input_, false);
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
        result.candidates = layout_->generate_candidates(raw_input_);
    }
    return result;
}

}  // namespace kalaime::layouts
