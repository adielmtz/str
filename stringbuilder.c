#include "stringbuilder.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <inttypes.h>
#include <ctype.h>

#define UINT64_MAX_STRLEN 20
#define LONG_FMT "%" PRId64
#define ULONG_FMT "%" PRIu64

#ifndef max
#define max(a, b) (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif

#define ENSURE_MEMORY_SIZE(sb, requiredSize) do {                                 \
    if (((requiredSize) + 1) > (sb)->size) {                                      \
        int32_t newSize = (sb)->size * 2;                                         \
        if (((requiredSize) + 1) > newSize) {                                     \
            newSize = ((requiredSize) + 1);                                       \
        }                                                                         \
        if (stringbuilder_set_size((sb), newSize) != STRING_BUILDER_ERROR_NONE) { \
            return (sb)->error;                                                   \
        }                                                                         \
    }                                                                             \
} while (0)

static void *(*mem_alloc)(size_t) = malloc;
static void *(*mem_realloc)(void *, size_t) = realloc;
static void (*mem_free)(void *) = free;

void stringbuilder_set_memory_alloc(void *(*malloc_fn)(size_t))
{
    mem_alloc = malloc_fn;
}

void stringbuilder_set_memory_realloc(void *(*realloc_fn)(void *, size_t))
{
    mem_realloc = realloc_fn;
}

void stringbuilder_set_memory_free(void (*free_fn)(void *))
{
    mem_free = free_fn;
}

StringBuilderError stringbuilder_init(StringBuilder *sb)
{
    return stringbuilder_init_size(sb, STRING_BUILDER_MIN_SIZE);
}

StringBuilderError stringbuilder_init_size(StringBuilder *sb, int32_t size)
{
    sb->error = STRING_BUILDER_ERROR_MEM_ALLOC_FAILURE;
    if (size > 0) {
        sb->str = mem_alloc(sizeof(char) * size);
        if (sb->str != NULL) {
            sb->len = 0;
            sb->size = size;
            sb->error = STRING_BUILDER_ERROR_NONE;
        }
    }

    return sb->error;
}

void stringbuilder_finalize(StringBuilder *sb)
{
    if (sb != NULL) {
        if (sb->str != NULL) {
            mem_free(sb->str);
        }

        sb->str = NULL;
        sb->len = 0;
        sb->size = 0;
        sb->error = STRING_BUILDER_ERROR_NONE;
    }
}

StringBuilderError stringbuilder_copy(StringBuilder *src, StringBuilder *dest)
{
    src->error = stringbuilder_init_size(dest, max(src->len + 1, STRING_BUILDER_MIN_SIZE));
    if (src->error == STRING_BUILDER_ERROR_NONE) {
        memcpy(dest->str, src->str, src->len);
        dest->len = src->len;
        dest->str[dest->len] = '\0';
    }

    return src->error;
}

const char *stringbuilder_get_str(const StringBuilder *sb)
{
    return sb->str;
}

StringBuilderError stringbuilder_get_last_error(const StringBuilder *sb)
{
    return sb->error;
}

const char *stringbuilder_get_error_msg(StringBuilderError code)
{
#define CASE_RETURN_ENUM_AS_STRING(val) case val: return #val
    switch (code) {
        CASE_RETURN_ENUM_AS_STRING(STRING_BUILDER_ERROR_NONE);
        CASE_RETURN_ENUM_AS_STRING(STRING_BUILDER_ERROR_MEM_ALLOC_FAILURE);
        CASE_RETURN_ENUM_AS_STRING(STRING_BUILDER_ERROR_OUT_OF_RANGE);
        default:
            return "Unknown error code";
    }
}

int32_t stringbuilder_get_length(const StringBuilder *sb)
{
    return sb->len;
}

StringBuilderError stringbuilder_set_length(StringBuilder *sb, int32_t length)
{
    ENSURE_MEMORY_SIZE(sb, length);
    if (length > sb->len) {
        char *dst = sb->str + sb->len;
        memset(dst, 0, length - sb->len);
    }

    sb->len = length;
    sb->str[length] = '\0';
    return sb->error;
}

int32_t stringbuilder_get_size(const StringBuilder *sb)
{
    return sb->size;
}

StringBuilderError stringbuilder_set_size(StringBuilder *sb, int32_t newSize)
{
    sb->error = STRING_BUILDER_ERROR_MEM_ALLOC_FAILURE;
    char *tmp = mem_realloc(sb->str, sizeof(char) * newSize);
    if (tmp != NULL) {
        sb->str = tmp;
        sb->size = newSize;
        sb->error = STRING_BUILDER_ERROR_NONE;
        if (sb->len >= sb->size) {
            sb->len = sb->size - 1;
            sb->str[sb->len] = '\0';
        }
    }

    return sb->error;
}

int stringbuilder_compare(const StringBuilder *a, const StringBuilder *b)
{
    if (a == b) {
        return 0;
    }

    int result = memcmp(a->str, b->str, min(a->len, b->len));
    if (!result) {
        result = (int) (a->len - b->len);
    }

    return result;
}

bool stringbuilder_equals(const StringBuilder *a, const StringBuilder *b)
{
    return a == b || a->len == b->len && memcmp(a->str, b->str, a->len) == 0;
}

bool stringbuilder_contains(const StringBuilder *sb, const char *needle, int32_t needle_len)
{
    // All strings contain an empty string ""
    if (needle_len == 0) {
        return true;
    }

    // Lookup for a single character in the string
    if (needle_len == 1) {
        return sb->len > 0 && memchr(sb->str, *needle, sb->len) != NULL;
    }

    // Lookup for a word in the string
    if (needle_len > 1 && needle_len <= sb->len) {
        char *curr = sb->str;
        char *end = sb->str + sb->len;

        do {
            curr = memchr(curr, *needle, end - curr);
            if (curr == NULL) {
                break;
            }

            if (memcmp(curr + 1, needle + 1, needle_len - 1) == 0) {
                return true;
            }

            curr += needle_len;
        } while (curr + needle_len < end);
    }

    return false;
}

bool stringbuilder_starts_with(const StringBuilder *sb, const char *prefix, int32_t prefix_len)
{
    if (sb->len < prefix_len) {
        return false;
    }

    return memcmp(sb->str, prefix, prefix_len) == 0;
}

bool stringbuilder_ends_with(const StringBuilder *sb, const char *suffix, int32_t suffix_len)
{
    if (sb->len < suffix_len) {
        return false;
    }

    const char *ptr = sb->str + sb->len - suffix_len;
    return memcmp(ptr, suffix, suffix_len) == 0;
}

StringBuilderError stringbuilder_concat(StringBuilder *sb, const StringBuilder *other)
{
    return stringbuilder_append_string(sb, other->str, other->len);
}

StringBuilderError stringbuilder_append_char(StringBuilder *sb, char c)
{
    ENSURE_MEMORY_SIZE(sb, sb->len + 1);
    sb->str[sb->len++] = c;
    sb->str[sb->len] = '\0';
    return sb->error;
}

StringBuilderError stringbuilder_append_string(StringBuilder *sb, const char *string, int32_t len)
{
    int32_t newLen = sb->len + len;
    ENSURE_MEMORY_SIZE(sb, newLen);
    char *dst = sb->str + sb->len;
    memcpy(dst, string, len);
    sb->len = newLen;
    sb->str[newLen] = '\0';
    return sb->error;
}

StringBuilderError stringbuilder_append_format(StringBuilder *sb, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    int chars = vsnprintf(NULL, 0, fmt, args);
    ENSURE_MEMORY_SIZE(sb, sb->len + chars);

    char *ptr = sb->str + sb->len;
    vsnprintf(ptr, sb->size - sb->len, fmt, args);
    va_end(args);
    sb->len += chars;
    sb->str[sb->len] = '\0';
    return sb->error;
}

static StringBuilderError internal_append_int(StringBuilder *sb, const char *fmt, uint64_t value)
{
    if (value <= (uint64_t) 9) {
        char c = (char) (value + '0');
        return stringbuilder_append_char(sb, c);
    }

    ENSURE_MEMORY_SIZE(sb, sb->len + UINT64_MAX_STRLEN);
    char *ptr = sb->str + sb->len;
    int chars = snprintf(ptr, sb->size - sb->len, fmt, value);
    sb->len += chars;
    sb->str[sb->len] = '\0';
    return sb->error;
}

StringBuilderError stringbuilder_append_int(StringBuilder *sb, int64_t value)
{
    return internal_append_int(sb, LONG_FMT, (uint64_t) value);
}

StringBuilderError stringbuilder_append_uint(StringBuilder *sb, uint64_t value)
{
    return internal_append_int(sb, ULONG_FMT, value);
}

StringBuilderError stringbuilder_append_float(StringBuilder *sb, double value, int32_t decimals)
{
    int chars = snprintf(NULL, 0, "%.*f", decimals, value);
    ENSURE_MEMORY_SIZE(sb, sb->len + chars);

    char *ptr = sb->str + sb->len;
    snprintf(ptr, sb->size - sb->len, "%.*f", decimals, value);
    sb->len += chars;
    sb->str[sb->len] = '\0';
    return sb->error;
}

static void internal_case_convert(StringBuilder *sb, int (*convert)(int))
{
    if (sb->len > 0) {
        char *c = sb->str;
        char *e = c + sb->len;

        while (c < e) {
            *c = (char) convert(*c);
            c++;
        }
    }
}

void stringbuilder_to_uppercase(StringBuilder *sb)
{
    internal_case_convert(sb, toupper);
}

void stringbuilder_to_lowercase(StringBuilder *sb)
{
    internal_case_convert(sb, tolower);
}

int stringbuilder_replace_char(StringBuilder *sb, char search, char replace)
{
    int n = 0;
    if (sb->len > 0) {
        char *c = sb->str;
        char *e = c + sb->len;
        while ((c = memchr(c, search, e - c)) != NULL) {
            *c = replace;
            c++;
            n++;
        }
    }

    return n;
}

StringBuilderError stringbuilder_repeat(StringBuilder *sb, int times)
{
    if (times < 0) {
        return sb->error = STRING_BUILDER_ERROR_OUT_OF_RANGE;
    }

    sb->error = STRING_BUILDER_ERROR_NONE;
    if (sb->len > 0) {
        if (times == 0) {
            sb->len = 0;
            sb->str[0] = '\0';
        } else {
            int32_t newLen = sb->len + (sb->len * (times - 1));
            ENSURE_MEMORY_SIZE(sb, newLen);

            char *dst = sb->str + sb->len;
            while (--times > 0) {
                memmove(dst, sb->str, sb->len);
                dst += sb->len;
            }

            sb->len = newLen;
            sb->str[newLen] = '\0';
        }
    }

    return sb->error;
}

void stringbuilder_trim(StringBuilder *sb)
{
    if (sb->len > 0) {
        char *c = sb->str;
        char *e = c + sb->len;

        // Trim the beginning of the string
        while (c < e && isspace(*c)) {
            c++;
        }

        if (c == e) {
            // The whole string is made of whitespace.
            // Set the length to 0 and return earlier
            sb->len = 0;
            sb->str[0] = '\0';
            return;
        }

        // Trim the end of the string
        e--; // Position into the last character (not the null terminator)
        while (e >= c && isspace(*e)) {
            e--;
        }

        int32_t newLen = ((int32_t) (e - c)) + 1;
        if (c > sb->str) {
            memmove(sb->str, c, newLen);
        }

        sb->len = newLen;
        sb->str[newLen] = '\0';
    }
}
