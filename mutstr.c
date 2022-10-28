#include "mutstr.h"

#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define UNSIGNED_INT_MAX_STRLEN 20
#define RETURN_CASE_AS_STRING(val) case val: return #val
#define SWITCH_UNREACHABLE_DEFAULT_CASE() default: assert(0)

#ifndef is_null
#define is_null(ptr) ((ptr) == NULL)
#endif

#ifndef max
#define max(a, b) (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif

#define MUTSTR_DEFAULT_INITIAL_SIZE 16
#define MUTSTR_TAIL_PTR(mutstr) ((mutstr)->value + (mutstr)->length)
#define MUTSTR_TAIL_VAL(mutstr) (*MUTSTR_TAIL_PTR(mutstr))

#define MUTSTR_CLEAR_MEMBERS(mutstr) \
do {                                 \
    (mutstr)->value = NULL;          \
    (mutstr)->length = 0;            \
    (mutstr)->size = 0;              \
} while (0)

#define MUTSTR_RETURN_IF_NULL(mutstr)                    \
do {                                                     \
    if (is_null((mutstr)) || is_null((mutstr)->value)) { \
        return MUTSTR_ERROR_NULL_POINTER;                \
    }                                                    \
} while (0)

const char *mutstr_get_state_msg(MutStrState state)
{
    switch (state) {
        RETURN_CASE_AS_STRING(MUTSTR_OK);
        RETURN_CASE_AS_STRING(MUTSTR_ERROR);
        RETURN_CASE_AS_STRING(MUTSTR_ERROR_NULL_POINTER);
        RETURN_CASE_AS_STRING(MUTSTR_ERROR_MALLOC_FAILED);
        RETURN_CASE_AS_STRING(MUTSTR_ERROR_UNKNOWN_LENGTH);
        RETURN_CASE_AS_STRING(MUTSTR_ERROR_INDEX_OUT_OF_RANGE);
        SWITCH_UNREACHABLE_DEFAULT_CASE();
    }
}

MutStrState mutstr_allocate(MutStr *result, int32_t size)
{
    if (is_null(result)) {
        return MUTSTR_ERROR_NULL_POINTER;
    }

    char *mem = malloc(sizeof(char) * size);
    if (is_null(mem)) {
        MUTSTR_CLEAR_MEMBERS(result);
        return MUTSTR_ERROR_MALLOC_FAILED;
    } else {
        mem[0] = '\0';
        result->value = mem;
        result->length = 0;
        result->size = size;
        return MUTSTR_OK;
    }
}

MutStrState mutstr_init(MutStr *result)
{
    return mutstr_allocate(result, MUTSTR_DEFAULT_INITIAL_SIZE);
}

void mutstr_finalize(MutStr *mutstr)
{
    if (!is_null(mutstr) && !is_null(mutstr->value)) {
        free(mutstr->value);
        MUTSTR_CLEAR_MEMBERS(mutstr);
    }
}

MutStrState mutstr_ensure_capacity(MutStr *mutstr, int32_t required_length)
{
    MUTSTR_RETURN_IF_NULL(mutstr);
    if (required_length > mutstr->size) {
        int32_t new_size = mutstr->size * 2;
        if (required_length > new_size) {
            new_size = required_length;
        }

        return mutstr_set_size(mutstr, new_size);
    }

    // allocated memory is large enough
    return MUTSTR_OK;
}

MutStrState mutstr_set_size(MutStr *mutstr, int32_t size)
{
    MUTSTR_RETURN_IF_NULL(mutstr);
    if (size < 0) {
        return MUTSTR_ERROR;
    }

    char *mem = realloc(mutstr->value, sizeof(char) * size);
    if (is_null(mem)) {
        return MUTSTR_ERROR_MALLOC_FAILED;
    } else {
        mutstr->value = mem;
        mutstr->size = size;

        if (size <= mutstr->length) {
            mutstr->length = size - 1;
            mutstr->value[mutstr->length] = '\0';
        }

        return MUTSTR_OK;
    }
}

MutStrState mutstr_set_length(MutStr *mutstr, int32_t length)
{
    MUTSTR_RETURN_IF_NULL(mutstr);
    if (length < 0) {
        return MUTSTR_ERROR;
    }

    MutStrState state = mutstr_ensure_capacity(mutstr, length + 1);
    if (state == MUTSTR_OK) {
        if (length > mutstr->length) {
            memset(MUTSTR_TAIL_PTR(mutstr), '\0', length - mutstr->length);
        }

        mutstr->length = length;
        mutstr->value[length] = '\0';
    }

    return state;
}

MutStrState mutstr_copy(const MutStr *source, MutStr *destination)
{
    MUTSTR_RETURN_IF_NULL(source);
    MUTSTR_RETURN_IF_NULL(destination);
    MutStrState state = mutstr_allocate(destination, source->length + 1);
    if (state == MUTSTR_OK) {
        memcpy(destination->value, source->value, source->length);
        destination->length = source->length;
        destination->value[destination->length] = '\0';
    }

    return state;
}

/**
 * Compares 2 strings of known length.
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

    return mutstr_memncmp(a->value, a->length, b->value, b->length);
}

int32_t mutstr_compare_string(const MutStr *mutstr, const char *string, int32_t length)
{
    return mutstr_memncmp(mutstr->value, mutstr->length, string, length);
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

int32_t mutstr_compare_literal(const MutStr *mutstr, const char *string)
{
    return mutstr_memncmp(mutstr->value, mutstr->length, string, mutstr_strnlen(string));
}

bool mutstr_equals(const MutStr *a, const MutStr *b)
{
    return a == b || (a->length == b->length && memcmp(a->value, b->value, a->length) == 0);
}

bool mutstr_equals_string(const MutStr *mutstr, const char *string, int32_t length)
{
    return mutstr->length == length && memcmp(mutstr->value, string, length) == 0;
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
    if (is_null(mutstr)) {
        return -1;
    }

    const char *s = mutstr_memnstr(mutstr->value, mutstr->length, needle, needle_len);
    return s == NULL ? -1 : (int32_t) (s - mutstr->value);
}

int32_t mutstr_indexof_literal(const MutStr *mutstr, const char *needle)
{
    return mutstr_indexof_string(mutstr, needle, mutstr_strnlen(needle));
}

bool mutstr_contains(const MutStr *mutstr, const MutStr *substr)
{
    return mutstr_contains_string(mutstr, substr->value, substr->length);
}

bool mutstr_contains_string(const MutStr *mutstr, const char *needle, int32_t needle_len)
{
    const char *result = mutstr_memnstr(mutstr->value, mutstr->length, needle, needle_len);
    return !is_null(result);
}

bool mutstr_contains_literal(const MutStr *mutstr, const char *needle)
{
    return mutstr_contains_string(mutstr, needle, mutstr_strnlen(needle));
}

bool mutstr_starts_with(const MutStr *mutstr, const MutStr *prefix)
{
    return mutstr_starts_with_string(mutstr, prefix->value, prefix->length);
}

bool mutstr_starts_with_string(const MutStr *mutstr, const char *prefix, int32_t prefix_len)
{
    return prefix_len >= 0 && memcmp(mutstr->value, prefix, prefix_len) == 0;
}

bool mutstr_starts_with_literal(const MutStr *mutstr, const char *prefix)
{
    return mutstr_starts_with_string(mutstr, prefix, mutstr_strnlen(prefix));
}

bool mutstr_ends_with(const MutStr *mutstr, const MutStr *suffix)
{
    return mutstr_ends_with_string(mutstr, suffix->value, suffix->length);
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
    return mutstr_append_string(mutstr, other->value, other->length);
}

MutStrState mutstr_append_char(MutStr *mutstr, char c)
{
    MUTSTR_RETURN_IF_NULL(mutstr);
    MutStrState state = mutstr_ensure_capacity(mutstr, mutstr->length + 2);

    if (state == MUTSTR_OK) {
        MUTSTR_TAIL_VAL(mutstr) = c;
        mutstr->length++;
        mutstr->value[mutstr->length] = '\0';
    }

    return state;
}

MutStrState mutstr_append_string(MutStr *mutstr, const char *str, int32_t length)
{
    MUTSTR_RETURN_IF_NULL(mutstr);
    int32_t new_length = mutstr->length + length;
    MutStrState state = mutstr_ensure_capacity(mutstr, new_length + 1);

    if (state == MUTSTR_OK) {
        memcpy(MUTSTR_TAIL_PTR(mutstr), str, length);
        mutstr->length = new_length;
        mutstr->value[new_length] = '\0';
    }

    return state;
}

MutStrState mutstr_append_literal(MutStr *mutstr, const char *str)
{
    return mutstr_append_string(mutstr, str, mutstr_strnlen(str));
}

MutStrState mutstr_append_format(MutStr *mutstr, const char *fmt, ...)
{
    MUTSTR_RETURN_IF_NULL(mutstr);
    va_list args;
    va_start(args, fmt);

    int32_t length = vsnprintf(NULL, 0, fmt, args);
    int32_t new_length = mutstr->length + length;
    MutStrState state = mutstr_ensure_capacity(mutstr, new_length + 1);

    if (state == MUTSTR_OK) {
        vsnprintf(MUTSTR_TAIL_PTR(mutstr), mutstr->size - mutstr->length, fmt, args);
        mutstr->length = new_length;
        mutstr->value[new_length] = '\0';
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

static void case_convert(char *s, int32_t n, int (*convert)(int))
{
    const char *e = s + n;
    while (s < e) {
        *s = (char) convert(*s);
        s++;
    }
}

MutStrState mutstr_to_uppercase(MutStr *mutstr)
{
    MUTSTR_RETURN_IF_NULL(mutstr);
    case_convert(mutstr->value, mutstr->length, toupper);
    return MUTSTR_OK;
}

MutStrState mutstr_to_lowercase(MutStr *mutstr)
{
    MUTSTR_RETURN_IF_NULL(mutstr);
    case_convert(mutstr->value, mutstr->length, tolower);
    return MUTSTR_OK;
}

MutStrState mutstr_trim(MutStr *mutstr, MutStrTrimOptions options)
{
    MUTSTR_RETURN_IF_NULL(mutstr);
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
    } else {
        // skip trailing null
        e--;
        if (options & MUTSTR_TRIM_RIGHT) {
            while (e > s && isspace(*e)) {
                e--;
            }
        }

        int32_t length = (int32_t) (e - s) + 1;
        if (s > mutstr->value) {
            memmove(mutstr->value, s, length);
        }

        mutstr->length = length;
        mutstr->value[length] = '\0';
    }

    return MUTSTR_OK;
}

MutStrState mutstr_substr(const MutStr *source, int32_t index, int32_t length, MutStr *result)
{
    MUTSTR_RETURN_IF_NULL(source);
    if (length < 0 || index < 0 || index >= source->length) {
        return MUTSTR_ERROR_INDEX_OUT_OF_RANGE;
    }

    const char *s = source->value + index;
    if (s + length >= MUTSTR_TAIL_PTR(source)) {
        length = MUTSTR_TAIL_PTR(source) - s;
    }

    MutStrState state = mutstr_allocate(result, length + 1);
    if (state == MUTSTR_OK) {
        memcpy(result->value, s, length);
        result->length = length;
        result->value[length] = '\0';
    }

    return state;
}

MutStrState mutstr_repeat(MutStr *mutstr, int32_t multiply)
{
    MUTSTR_RETURN_IF_NULL(mutstr);
    if (multiply == 0 || mutstr->length == 0) {
        // truncate to empty
        mutstr->value[0] = '\0';
        mutstr->length = 0;
        return MUTSTR_OK;
    }

    int32_t length = mutstr->length * multiply;
    MutStrState state = mutstr_ensure_capacity(mutstr, length + 1);
    if (state == MUTSTR_OK) {
        if (mutstr->length == 1) {
            memset(mutstr->value, mutstr->value[0], length);
        } else {
            char *s = MUTSTR_TAIL_PTR(mutstr);
            const char *e = mutstr->value + length;

            while (s < e) {
                memmove(s, mutstr->value, mutstr->length);
                s += mutstr->length;
            }
        }

        mutstr->value[length] = '\0';
        mutstr->length = length;
    }

    return state;
}
