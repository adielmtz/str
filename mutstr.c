#include "mutstr.h"

#include <assert.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define min(a, b) (((a) < (b)) ? (a) : (b))
#define max(a, b) (((a) > (b)) ? (a) : (b))

#define UNSIGNED_INT_MAX_STRLEN 20

#define RETURN_CASE_AS_STRING(val) case val: return #val
#define SWITCH_UNREACHABLE_DEFAULT_CASE() default: assert(0)

#define MUTSTR_DEFAULT_INITIAL_SIZE 16
#define MUTSTR_TAIL_PTR(mutstr) ((mutstr)->val + (mutstr)->len)
#define MUTSTR_TAIL_VAL(mutstr) (*MUTSTR_TAIL_PTR(mutstr))

#define MUTSTR_CLEAR(mutstr) \
do {                         \
    (mutstr)->val = NULL;    \
    (mutstr)->len = 0;       \
    (mutstr)->size = 0;      \
} while (0)

const char *mutstr_get_state_msg(MutStrState state)
{
    switch (state) {
        RETURN_CASE_AS_STRING(MUTSTR_OK);
        RETURN_CASE_AS_STRING(MUTSTR_ERROR);
        RETURN_CASE_AS_STRING(MUTSTR_NO_MEMORY);
        RETURN_CASE_AS_STRING(MUTSTR_OUT_OF_RANGE);
        SWITCH_UNREACHABLE_DEFAULT_CASE();
    }
}

MutStrState mutstr_allocate(MutStr *result, int32_t size)
{
    MUTSTR_CLEAR(result);
    if (size > 0) {
        char *mem = malloc(sizeof(char) * size);
        if (mem) {
            mem[0] = '\0';
            result->val = mem;
            result->len = 0;
            result->size = size;
            return MUTSTR_OK;
        }
    }

    return MUTSTR_NO_MEMORY;
}

MutStrState mutstr_init(MutStr *result)
{
    return mutstr_allocate(result, MUTSTR_DEFAULT_INITIAL_SIZE);
}

void mutstr_finalize(MutStr *mutstr)
{
    if (mutstr && mutstr->val) {
        free(mutstr->val);
        MUTSTR_CLEAR(mutstr);
    }
}

MutStrState mutstr_set_size(MutStr *mutstr, int32_t size)
{
    if (size > 0) {
        char *mem = realloc(mutstr->val, sizeof(char) * size);
        if (mem) {
            mutstr->val = mem;
            mutstr->size = size;

            if (size <= mutstr->len) {
                mutstr->len = size - 1;
                mutstr->val[mutstr->len] = '\0';
            }

            return MUTSTR_OK;
        }
    }

    return MUTSTR_NO_MEMORY;
}

MutStrState mutstr_set_length(MutStr *mutstr, int32_t length)
{
    if (length < 0) {
        return MUTSTR_OUT_OF_RANGE;
    }

    MutStrState state = mutstr_ensure_capacity(mutstr, length + 1);
    if (state == MUTSTR_OK) {
        if (length > mutstr->len) {
            memset(MUTSTR_TAIL_PTR(mutstr), '\0', length - mutstr->len);
        }

        mutstr->len = length;
        mutstr->val[length] = '\0';
    }

    return state;
}

MutStrState mutstr_ensure_capacity(MutStr *mutstr, int32_t length)
{
    if (length > mutstr->size) {
        int32_t new_size = mutstr->size * 2;
        if (length > new_size) {
            new_size = length;
        }

        return mutstr_set_size(mutstr, new_size);
    }

    /* Current allocated memory is large enough */
    return MUTSTR_OK;
}

MutStrState mutstr_copy(const MutStr *source, MutStr *destination)
{
    MutStrState state = mutstr_allocate(destination, source->len + 1);
    if (state == MUTSTR_OK) {
        memcpy(destination->val, source->val, source->len);
        destination->len = source->len;
        destination->val[destination->len] = '\0';
    }

    return state;
}

/**
 * Compares 2 strings.
 */
static int32_t mutstr_memncmp(const char *a, int32_t a_len, const char *b, int32_t b_len)
{
    int32_t result = memcmp(a, b, min(a_len, b_len));
    if (!result) {
        result = a_len - b_len;
    }

    return result;
}

int32_t mutstr_compare(const MutStr *a, const MutStr *b)
{
    if (a == b) {
        return 0;
    }

    return mutstr_memncmp(a->val, a->len, b->val, b->len);
}

/**
 * Calculates the length of the given string.
 * Returns INT32_MAX if the string is not null terminated.
 */
static int32_t mutstr_strnlen(const char *s)
{
    const char *n = memchr(s, '\0', INT32_MAX);
    return n == NULL ? INT32_MAX : (int32_t) (n - s);
}

int32_t mutstr_compare_string(const MutStr *mutstr, const char *string, int32_t length)
{
    return mutstr_memncmp(mutstr->val, mutstr->len, string, length);
}

int32_t mutstr_compare_literal(const MutStr *mutstr, const char *string)
{
    return mutstr_memncmp(mutstr->val, mutstr->len, string, mutstr_strnlen(string));
}

bool mutstr_equals(const MutStr *a, const MutStr *b)
{
    return a == b || (a->len == b->len && memcmp(a->val, b->val, a->len) == 0);
}

bool mutstr_equals_string(const MutStr *mutstr, const char *string, int32_t length)
{
    return mutstr->len == length && memcmp(mutstr->val, string, length) == 0;
}

bool mutstr_equals_literal(const MutStr *mutstr, const char *string)
{
    return mutstr_equals_string(mutstr, string, mutstr_strnlen(string));
}

static char *mutstr_memnstr(char *haystack, int32_t haystack_len, const char *needle, int32_t needle_len)
{
    if (needle_len == 0) {
        return haystack;
    }

    if (haystack_len >= needle_len) {
        if (needle_len == 1) {
            return memchr(haystack, *needle, haystack_len);
        } else {
            const char *end = haystack + haystack_len;
            haystack = memchr(haystack, *needle, end - haystack);

            while (haystack != NULL && haystack + needle_len <= end) {
                if (memcmp(haystack, needle, needle_len) == 0) {
                    return haystack;
                }

                haystack = memchr(haystack + needle_len, *needle, end - haystack + needle_len);
            }
        }
    }

    return NULL;
}

int32_t mutstr_indexof_string(const MutStr *mutstr, const char *needle, int32_t needle_len)
{
    const char *s = mutstr_memnstr(mutstr->val, mutstr->len, needle, needle_len);
    return s == NULL ? -1 : (int32_t) (s - mutstr->val);
}

int32_t mutstr_indexof_literal(const MutStr *mutstr, const char *needle)
{
    return mutstr_indexof_string(mutstr, needle, mutstr_strnlen(needle));
}

bool mutstr_contains(const MutStr *mutstr, const MutStr *substr)
{
    return mutstr_contains_string(mutstr, substr->val, substr->len);
}

bool mutstr_contains_string(const MutStr *mutstr, const char *needle, int32_t needle_len)
{
    const char *result = mutstr_memnstr(mutstr->val, mutstr->len, needle, needle_len);
    return result != NULL;
}

bool mutstr_contains_literal(const MutStr *mutstr, const char *needle)
{
    return mutstr_contains_string(mutstr, needle, mutstr_strnlen(needle));
}

bool mutstr_starts_with(const MutStr *mutstr, const MutStr *prefix)
{
    return mutstr_starts_with_string(mutstr, prefix->val, prefix->len);
}

bool mutstr_starts_with_string(const MutStr *mutstr, const char *prefix, int32_t prefix_len)
{
    return prefix_len >= 0 && memcmp(mutstr->val, prefix, prefix_len) == 0;
}

bool mutstr_starts_with_literal(const MutStr *mutstr, const char *prefix)
{
    return mutstr_starts_with_string(mutstr, prefix, mutstr_strnlen(prefix));
}

bool mutstr_ends_with(const MutStr *mutstr, const MutStr *suffix)
{
    return mutstr_ends_with_string(mutstr, suffix->val, suffix->len);
}

bool mutstr_ends_with_string(const MutStr *mutstr, const char *suffix, int32_t suffix_len)
{
    return suffix_len >= 0 && memcmp(MUTSTR_TAIL_PTR(mutstr) - suffix_len, suffix, suffix_len) == 0;
}

bool mutstr_ends_with_literal(const MutStr *mutstr, const char *suffix)
{
    return mutstr_ends_with_string(mutstr, suffix, mutstr_strnlen(suffix));
}

MutStrState mutstr_append(MutStr *mutstr, const MutStr *other)
{
    return mutstr_append_string(mutstr, other->val, other->len);
}

MutStrState mutstr_append_char(MutStr *mutstr, char c)
{
    MutStrState state = mutstr_ensure_capacity(mutstr, mutstr->len + 2);
    if (state == MUTSTR_OK) {
        MUTSTR_TAIL_VAL(mutstr) = c;
        mutstr->len++;
        mutstr->val[mutstr->len] = '\0';
    }

    return state;
}

MutStrState mutstr_append_string(MutStr *mutstr, const char *str, int32_t length)
{
    int32_t new_length = mutstr->len + length;
    MutStrState state = mutstr_ensure_capacity(mutstr, new_length + 1);

    if (state == MUTSTR_OK) {
        memcpy(MUTSTR_TAIL_PTR(mutstr), str, length);
        mutstr->len = new_length;
        mutstr->val[new_length] = '\0';
    }

    return state;
}

MutStrState mutstr_append_literal(MutStr *mutstr, const char *str)
{
    return mutstr_append_string(mutstr, str, mutstr_strnlen(str));
}

MutStrState mutstr_append_format(MutStr *mutstr, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    int32_t length = vsnprintf(NULL, 0, fmt, args);
    int32_t new_length = mutstr->len + length;
    MutStrState state = mutstr_ensure_capacity(mutstr, new_length + 1);

    if (state == MUTSTR_OK) {
        vsnprintf(MUTSTR_TAIL_PTR(mutstr), mutstr->size - mutstr->len, fmt, args);
        mutstr->len = new_length;
        mutstr->val[new_length] = '\0';
    }

    va_end(args);
    return state;
}

static char *uint_to_str(char *buffer, uint64_t n)
{
    *buffer = '\0';
    do {
        *--buffer = (char) ((n % 10) + '0');
        n /= 10;
    } while (n > 0);
    return buffer;
}

static char *int_to_str(char *buffer, int64_t n)
{
    if (n < 0) {
        char *result = uint_to_str(buffer, ~((uint64_t) n) + 1);
        *--result = '-';
        return result;
    } else {
        return uint_to_str(buffer, n);
    }
}

MutStrState mutstr_append_int(MutStr *mutstr, int64_t value)
{
    if ((uint64_t) value <= 9) {
        return mutstr_append_char(mutstr, (char) ('0' + value));
    } else {
        char buffer[UNSIGNED_INT_MAX_STRLEN + 1];
        char *result = int_to_str(buffer + sizeof(buffer) - 1, value);
        return mutstr_append_string(mutstr, result, (int32_t) (buffer + sizeof(buffer) - 1 - result));
    }
}

MutStrState mutstr_append_uint(MutStr *mutstr, uint64_t value)
{
    if (value <= 9) {
        return mutstr_append_char(mutstr, (char) ('0' + value));
    } else {
        char buffer[UNSIGNED_INT_MAX_STRLEN + 1];
        char *result = uint_to_str(buffer + sizeof(buffer) - 1, value);
        return mutstr_append_string(mutstr, result, (int32_t) (buffer + sizeof(buffer) - 1 - result));
    }
}

MutStrState mutstr_append_float(MutStr *mutstr, double value, int32_t precision)
{
    return mutstr_append_format(mutstr, "%.*f", precision, value);
}

static void case_convert(const MutStr *mutstr, int (*convert)(int))
{
    char *s = mutstr->val;
    const char *e = MUTSTR_TAIL_PTR(mutstr);
    while (s < e) {
        *s = (char) convert(*s);
        s++;
    }
}

void mutstr_to_uppercase(MutStr *mutstr)
{
    case_convert(mutstr, toupper);
}

void mutstr_to_lowercase(MutStr *mutstr)
{
    case_convert(mutstr, tolower);
}

MutStrState mutstr_trim(MutStr *mutstr, MutStrTrimOptions options)
{
    const char *s = mutstr->val;
    const char *e = MUTSTR_TAIL_PTR(mutstr);

    if (options & MUTSTR_TRIM_LEFT) {
        while (s < e && isspace(*s)) {
            s++;
        }
    }

    if (s == e) {
        // truncate to empty string
        mutstr->val[0] = '\0';
        mutstr->len = 0;
    } else {
        // skip trailing null
        e--;
        if (options & MUTSTR_TRIM_RIGHT) {
            while (e > s && isspace(*e)) {
                e--;
            }
        }

        int32_t length = (int32_t) (e - s) + 1;
        if (s > mutstr->val) {
            memmove(mutstr->val, s, length);
        }

        mutstr->len = length;
        mutstr->val[length] = '\0';
    }

    return MUTSTR_OK;
}

MutStrState mutstr_substr(const MutStr *source, int32_t index, int32_t length, MutStr *result)
{
    if (length < 0 || index < 0 || index >= source->len) {
        return MUTSTR_OUT_OF_RANGE;
    }

    const char *s = source->val + index;
    if (s + length >= MUTSTR_TAIL_PTR(source)) {
        length = MUTSTR_TAIL_PTR(source) - s;
    }

    MutStrState state = mutstr_allocate(result, length + 1);
    if (state == MUTSTR_OK) {
        memcpy(result->val, s, length);
        result->len = length;
        result->val[length] = '\0';
    }

    return state;
}

MutStrState mutstr_repeat(MutStr *mutstr, int32_t multiply)
{
    if (multiply == 0 || mutstr->len == 0) {
        // truncate to empty
        mutstr->val[0] = '\0';
        mutstr->len = 0;
        return MUTSTR_OK;
    }

    int32_t length = mutstr->len * multiply;
    MutStrState state = mutstr_ensure_capacity(mutstr, length + 1);
    if (state == MUTSTR_OK) {
        if (mutstr->len == 1) {
            memset(mutstr->val, mutstr->val[0], length);
        } else {
            char *s = MUTSTR_TAIL_PTR(mutstr);
            const char *e = mutstr->val + length;

            while (s < e) {
                memmove(s, mutstr->val, mutstr->len);
                s += mutstr->len;
            }
        }

        mutstr->val[length] = '\0';
        mutstr->len = length;
    }

    return state;
}
