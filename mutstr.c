#include "mutstr.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <ctype.h>

#define INT64_FORMAT_STRING "%" PRId64
#define UINT64_FORMAT_STRING "%" PRIu64

#define MUTSTR_TAIL_PTR(mutstr) ((mutstr)->value + (mutstr)->length)
#define MUTSTR_TAIL_VAL(mutstr) (*MUTSTR_TAIL_PTR(mutstr))

#ifndef max
#define max(a, b) (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif

#define MUTSTR_CLEAR_STATE(mutstr) \
do {                               \
    (mutstr)->state = MUTSTR_OK;   \
} while (0)

#define MUTSTR_CLEAR_MEMBERS(mutstr)        \
do {                                        \
    (mutstr)->value = NULL;                 \
    (mutstr)->length = 0;                   \
    (mutstr)->size = 0;                     \
    (mutstr)->state = MUTSTR_UNINITIALIZED; \
} while (0)

static int32_t safe_strlen(const char *s)
{
    const char *n = memchr(s, '\0', INT32_MAX);
    return n == NULL ? -1 : ((int32_t) (n - s));
}

void mutstr_init(MutStr *mutstr)
{
    mutstr_init_size(mutstr, MUTSTR_DEFAULT_INITIAL_SIZE);
}

void mutstr_init_size(MutStr *mutstr, int32_t initial_size)
{
    MUTSTR_CLEAR_MEMBERS(mutstr);
    char *str = malloc(sizeof(char) * initial_size);
    if (str != NULL) {
        str[0] = '\0';
        mutstr->value = str;
        mutstr->size = initial_size;
        mutstr->state = MUTSTR_OK;
    }
}

void mutstr_finalize(MutStr *mutstr)
{
    if (mutstr->state != MUTSTR_UNINITIALIZED) {
        free(mutstr->value);
        MUTSTR_CLEAR_MEMBERS(mutstr);
    }
}

void mutstr_set_length(MutStr *mutstr, int32_t length)
{
    if (length < 0) {
        mutstr->state = MUTSTR_MEMORY_ALLOC_FAIL;
        return;
    }

    mutstr_ensure_size(mutstr, length + 1);
    if (mutstr->state == MUTSTR_OK) {
        if (length > mutstr->length) {
            // fill gap
            memset(MUTSTR_TAIL_PTR(mutstr), '\0', length - mutstr->length);
        }

        mutstr->length = length;
        mutstr->value[length] = '\0';
    }
}

void mutstr_set_size(MutStr *mutstr, int32_t size)
{
    if (size <= 0) {
        mutstr->state = MUTSTR_MEMORY_ALLOC_FAIL;
        return;
    }

    MUTSTR_CLEAR_STATE(mutstr);
    char *tmp = realloc(mutstr->value, sizeof(char) * size);
    if (tmp == NULL) {
        mutstr->state = MUTSTR_MEMORY_ALLOC_FAIL;
        return;
    }

    mutstr->value = tmp;
    mutstr->size = size;
    if (size <= mutstr->length) {
        // truncate string
        mutstr->length = size - 1;
        mutstr->value[mutstr->length] = '\0';
    }
}

void mutstr_ensure_size(MutStr *mutstr, int32_t required_size)
{
    MUTSTR_CLEAR_STATE(mutstr);
    if (required_size > mutstr->size) {
        int32_t new_size = mutstr->size * 2;
        if (required_size > new_size) {
            new_size = required_size;
        }

        mutstr_set_size(mutstr, new_size);
    }
}

void mutstr_copy(const MutStr *source, MutStr *destination)
{
    mutstr_init_size(destination, source->length + 1);
    if (destination->state == MUTSTR_OK) {
        memcpy(destination->value, source->value, source->length);
        destination->length = source->length;
        destination->value[destination->length] = '\0';
    }
}

int32_t mutstr_compare(const MutStr *a, const MutStr *b)
{
    if (a == b) {
        return 0;
    }

    int32_t result = memcmp(a->value, b->value, min(a->length, b->length));
    if (!result) {
        result = (int32_t) (a->length - b->length);
    }

    return result;
}

bool mutstr_equals(const MutStr *a, const MutStr *b)
{
    return a == b || (a->length == b->length && memcmp(a->value, b->value, a->length) == 0);
}

static char *internal_find_substring_offset(char *str, int32_t str_len, const char *substr, int32_t substr_len)
{
    if (substr_len == 0) {
        // all strings contain an empty string
        return str;
    }

    if (str_len > 0) {
        if (substr_len == 1) {
            return memchr(str, substr[0], str_len);
        } else if (substr_len > 1 && substr_len <= str_len) {
            char *current = str;
            const char *end = str + str_len;

            do {
                current = memchr(current, substr[0], end - current);
                if (current == NULL) {
                    break;
                }

                if (memcmp(current + 1, substr + 1, substr_len - 1) == 0) {
                    return current;
                }

                current += substr_len;
            } while (current + substr_len < end);
        }
    }

    // substr not found
    return NULL;
}

int32_t mutstr_indexof(const MutStr *mutstr, const char *substr)
{
    int32_t substr_len = safe_strlen(substr);
    if (substr_len == -1) {
        return -1;
    }

    char *s = internal_find_substring_offset(mutstr->value, mutstr->length, substr, substr_len);
    return (int32_t) (s - mutstr->value);
}

bool mutstr_contains(const MutStr *mutstr, const char *substr)
{
    int32_t substr_len = safe_strlen(substr);
    if (substr_len == -1) {
        return false;
    }

    return internal_find_substring_offset(mutstr->value, mutstr->length, substr, substr_len);
}

bool mutstr_starts_with(const MutStr *mutstr, const char *prefix)
{
    int32_t prefix_len = safe_strlen(prefix);
    return prefix_len > -1 && memcmp(mutstr->value, prefix, prefix_len) == 0;
}

bool mutstr_ends_with(const MutStr *mutstr, const char *suffix)
{
    int32_t suffix_len = safe_strlen(suffix);
    return suffix_len > -1 && memcmp(MUTSTR_TAIL_PTR(mutstr) - suffix_len, suffix, suffix_len) == 0;
}

void mutstr_append_mutstr(MutStr *mutstr, const MutStr *other)
{
    mutstr_append_string(mutstr, other->value, other->length);
}

void mutstr_append_char(MutStr *mutstr, char c)
{
    MUTSTR_CLEAR_STATE(mutstr);
    mutstr_ensure_size(mutstr, mutstr->length + 2);
    if (mutstr->state == MUTSTR_OK) {
        MUTSTR_TAIL_VAL(mutstr) = c;
        mutstr->length++;
        mutstr->value[mutstr->length] = '\0';
    }
}

void mutstr_append_string(MutStr *mutstr, const char *str, int32_t length)
{
    int32_t new_length = mutstr->length + length;
    mutstr_ensure_size(mutstr, new_length + 1);
    if (mutstr->state == MUTSTR_OK) {
        memcpy(MUTSTR_TAIL_PTR(mutstr), str, length);
        mutstr->length = new_length;
        mutstr->value[new_length] = '\0';
    }
}

void mutstr_append_literal(MutStr *mutstr, const char *str)
{
    int32_t length = safe_strlen(str);
    if (length == -1) {
        mutstr->state = MUTSTR_UNKNOWN_LENGTH;
    } else {
        mutstr_append_string(mutstr, str, length);
    }
}

void mutstr_append_format(MutStr *mutstr, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    int32_t length = vsnprintf(NULL, 0, fmt, args);
    int32_t new_length = mutstr->length + length;
    mutstr_ensure_size(mutstr, new_length + 1);

    if (mutstr->state == MUTSTR_OK) {
        vsnprintf(MUTSTR_TAIL_PTR(mutstr), mutstr->size - mutstr->length, fmt, args);
        mutstr->length = new_length;
        mutstr->value[new_length] = '\0';
    }
}

void mutstr_append_int(MutStr *mutstr, int64_t value)
{
    mutstr_append_format(mutstr, INT64_FORMAT_STRING, value);
}

void mutstr_append_uint(MutStr *mutstr, uint64_t value)
{
    mutstr_append_format(mutstr, UINT64_FORMAT_STRING, value);
}

void mutstr_append_float(MutStr *mutstr, double value, int32_t precision)
{
    mutstr_append_format(mutstr, "%.*f", precision, value);
}

static void internal_case_convert(MutStr *mutstr, int (*convert)(int))
{
    MUTSTR_CLEAR_STATE(mutstr);
    if (mutstr->length > 0) {
        char *c = mutstr->value;
        const char *e = MUTSTR_TAIL_PTR(mutstr);

        while (c < e) {
            *c = (char) convert(*c);
            c++;
        }
    }
}

void mutstr_to_uppercase(MutStr *mutstr)
{
    internal_case_convert(mutstr, toupper);
}

void mutstr_to_lowercase(MutStr *mutstr)
{
    internal_case_convert(mutstr, tolower);
}

void mutstr_trim(MutStr *mutstr, MutStrTrimOptions options)
{
    MUTSTR_CLEAR_STATE(mutstr);
    const char *s = mutstr->value;
    const char *e = MUTSTR_TAIL_PTR(mutstr);

    if (options & MUTSTR_TRIM_LEFT) {
        while (s < e && isspace(*s)) {
            s++;
        }
    }

    if (s == e) {
        // truncate to empty string
        mutstr->value[0] = '\0';
        mutstr->length = 0;
        return;
    }

    // skip trailing null byte
    e--;
    if (options & MUTSTR_TRIM_RIGHT) {
        while (e > s && isspace(*e)) {
            e--;
        }
    }

    int32_t length = ((int32_t) (e - s)) + 1;
    if (s > mutstr->value) {
        memmove(mutstr->value, s, length);
    }

    mutstr->length = length;
    mutstr->value[length] = '\0';
}
