#pragma once

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_WIN32)
#if defined(SUBORNO_API_EXPORTS)
#define SUBORNO_API __declspec(dllexport)
#else
#define SUBORNO_API __declspec(dllimport)
#endif
#else
#define SUBORNO_API
#endif

typedef struct SUBORNO_Engine SUBORNO_Engine;
typedef struct SUBORNO_Session SUBORNO_Session;

SUBORNO_API SUBORNO_Engine* suborno_engine_create(void);
SUBORNO_API void suborno_engine_destroy(SUBORNO_Engine* engine);
SUBORNO_API SUBORNO_Session* suborno_session_create(SUBORNO_Engine* engine);
SUBORNO_API void suborno_session_destroy(SUBORNO_Session* session);
SUBORNO_API int suborno_session_append_utf8(SUBORNO_Session* session, const char* input);
SUBORNO_API int suborno_session_backspace(SUBORNO_Session* session);
SUBORNO_API int suborno_session_snapshot(SUBORNO_Session* session, char* buffer,
                                         size_t buffer_size);
SUBORNO_API int suborno_session_commit(SUBORNO_Session* session, char** committed,
                                       size_t* committed_size);
SUBORNO_API int suborno_session_commit_candidate(SUBORNO_Session* session, size_t index,
                                                 char** committed, size_t* committed_size);
SUBORNO_API void suborno_string_free(char* text);

#ifdef __cplusplus
}
#endif
