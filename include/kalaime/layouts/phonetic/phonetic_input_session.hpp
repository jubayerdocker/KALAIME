#pragma once

#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

#include "kalaime/layout-api/phonetic_layout.hpp"

namespace kalaime::layouts {

enum class InputStatus {
    updated,
    pending,
    literal_passthrough,
    invalid_utf8,
    input_limit,
    candidate_out_of_range,
    empty,
};

struct CompositionSnapshot {
    std::string raw_input_utf8;
    std::string preedit_utf8;
    std::vector<kalaime::layout::Candidate> candidates;
    InputStatus status = InputStatus::empty;
};

struct CommitResult {
    std::string text_utf8;
    InputStatus status = InputStatus::empty;
};

class PhoneticInputSession final {
   public:
    static constexpr std::size_t kMaxInputCodePoints = 256;

    PhoneticInputSession();
    explicit PhoneticInputSession(const kalaime::layout::PhoneticLayout& layout);

    // Each session owns mutable composition state and is thread-confined.
    InputStatus append_utf8(std::string_view input);
    InputStatus backspace();
    CommitResult commit();
    CommitResult commit_candidate(std::size_t index);
    void reset() noexcept;
    CompositionSnapshot snapshot() const;

   private:
    const kalaime::layout::PhoneticLayout* layout_ = nullptr;
    std::string raw_input_;
};

}  // namespace kalaime::layouts
