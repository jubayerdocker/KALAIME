#include <cstdlib>
#include <cstring>

#include "suborno/core/engine.hpp"
#include "suborno/layouts/phonetic/phonetic_input_session.hpp"
#include "suborno/suborno.h"

struct SUBORNO_Engine {
    suborno::core::Engine engine;
};

struct SUBORNO_Session {
    std::unique_ptr<suborno::layouts::PhoneticInputSession> session;
};

extern "C" {

SUBORNO_Engine* suborno_engine_create(void) {
    try {
        return new SUBORNO_Engine{};
    } catch (...) {
        return nullptr;
    }
}

void suborno_engine_destroy(SUBORNO_Engine* engine) { delete engine; }

SUBORNO_Session* suborno_session_create(SUBORNO_Engine* engine) {
    if (engine == nullptr) {
        return nullptr;
    }
    try {
        auto session = engine->engine.create_session();
        if (session == nullptr) {
            return nullptr;
        }
        return new SUBORNO_Session{std::move(session)};
    } catch (...) {
        return nullptr;
    }
}

void suborno_session_destroy(SUBORNO_Session* session) { delete session; }

int suborno_session_append_utf8(SUBORNO_Session* session, const char* input) {
    if (session == nullptr || input == nullptr) {
        return -1;
    }
    return static_cast<int>(session->session->append_utf8(input));
}

int suborno_session_backspace(SUBORNO_Session* session) {
    if (session == nullptr) {
        return -1;
    }
    return static_cast<int>(session->session->backspace());
}

int suborno_session_snapshot(SUBORNO_Session* session, char* buffer, size_t buffer_size) {
    if (session == nullptr || buffer == nullptr || buffer_size == 0) {
        return -1;
    }
    const auto snapshot = session->session->snapshot();
    if (snapshot.preedit_utf8.size() >= buffer_size) {
        buffer[0] = '\0';
        return -2;
    }
    std::memcpy(buffer, snapshot.preedit_utf8.data(), snapshot.preedit_utf8.size());
    buffer[snapshot.preedit_utf8.size()] = '\0';
    return static_cast<int>(snapshot.status);
}

int suborno_session_commit(SUBORNO_Session* session, char** committed, size_t* committed_size) {
    if (session == nullptr || committed == nullptr || committed_size == nullptr) {
        return -1;
    }
    const auto result = session->session->commit();
    *committed_size = result.text_utf8.size();
    *committed = static_cast<char*>(std::malloc(*committed_size + 1));
    if (*committed == nullptr) {
        return -1;
    }
    std::memcpy(*committed, result.text_utf8.data(), *committed_size);
    (*committed)[*committed_size] = '\0';
    return static_cast<int>(result.status);
}

int suborno_session_commit_candidate(SUBORNO_Session* session, size_t index, char** committed,
                                     size_t* committed_size) {
    if (session == nullptr || committed == nullptr || committed_size == nullptr) {
        return -1;
    }
    const auto result = session->session->commit_candidate(index);
    *committed_size = result.text_utf8.size();
    *committed = static_cast<char*>(std::malloc(*committed_size + 1));
    if (*committed == nullptr) {
        return -1;
    }
    std::memcpy(*committed, result.text_utf8.data(), *committed_size);
    (*committed)[*committed_size] = '\0';
    return static_cast<int>(result.status);
}

void suborno_string_free(char* text) { std::free(text); }

}  // extern "C"
