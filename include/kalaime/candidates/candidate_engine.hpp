#pragma once

#include <string>
#include <vector>

#include "kalaime/layout-api/phonetic_layout.hpp"

namespace kalaime::dictionary {
class Dictionary;
}

namespace kalaime::candidates {

class CandidateEngine {
   public:
    explicit CandidateEngine(const kalaime::layout::PhoneticLayout& layout);
    CandidateEngine(const kalaime::layout::PhoneticLayout& layout,
                    const kalaime::dictionary::Dictionary& dictionary);

    std::vector<kalaime::layout::Candidate> make_candidates(const std::string& input) const;

   private:
    const kalaime::layout::PhoneticLayout& layout_;
    const kalaime::dictionary::Dictionary* dictionary_ = nullptr;
};

}  // namespace kalaime::candidates
