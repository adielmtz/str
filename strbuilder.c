#include "strbuilder.h"

#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <inttypes.h>
#include <float.h>
#include <ctype.h>

#define UINT64_MAX_STRLEN 21
#define DOUBLE_MAX_STRLEN (3 + DBL_MANT_DIG - DBL_MIN_EXP)
#define LONG_FMT "%" PRId64
#define ULONG_FMT "%" PRIu64

#define CASE_RETURN_ENUM_STR(val) case val: return #val

#ifndef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef MAX
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif

#define SET_ERROR_RETURN(sb, value) do { \
    (sb)->err = (value);                 \
    return (value);                      \
} while (0)

#define GROW_STR(sb, requiredSize) do {                              \
    if ((requiredSize) > (sb)->size) {                               \
        size_t newSize = (sb)->size * 2;                             \
        if ((requiredSize) > newSize) {                              \
            newSize = (requiredSize);                                \
        }                                                            \
        if (!strbuilder_reallocate_str((sb), newSize)) {             \
            SET_ERROR_RETURN(sb, STRBUILDER_ERROR_MEM_ALLOC_FAILED); \
        }                                                            \
    }                                                                \
} while (0)

static void *(*mem_allocate)(size_t) = malloc;

static void *(*mem_reallocate)(void *, size_t) = realloc;

static void (*mem_free)(void *) = free;

static bool strbuilder_reallocate_str(StrBuilder *sb, size_t newSize)
{
    char *tmp = mem_reallocate(sb->str, sizeof(char) * newSize);
    if (tmp != NULL) {
        sb->str = tmp;
        sb->size = newSize;
        if (sb->len > newSize) {
            sb->len = newSize;
        }

        return true;
    }

    return false;
}

void strbuilder_set_mem_allocator(void *(*mem_alloc_fn)(size_t))
{
    mem_allocate = mem_alloc_fn;
}

void strbuilder_set_mem_reallocator(void *(*mem_realloc_fn)(void *, size_t))
{
    mem_reallocate = mem_realloc_fn;
}

void strbuilder_set_mem_free(void (*mem_free_fn)(void *))
{
    mem_free = mem_free_fn;
}

StrBuilderErr strbuilder_init(StrBuilder *sb)
{
    return strbuilder_init_sz(sb, STRBUILDER_DEFAULT_SIZE);
}

StrBuilderErr strbuilder_init_sz(StrBuilder *sb, size_t size)
{
    sb->str = mem_allocate(sizeof(char) * size);
    if (sb->str != NULL) {
        sb->size = size;
        sb->len = 0;
        SET_ERROR_RETURN(sb, STRBUILDER_ERROR_NONE);
    }

    SET_ERROR_RETURN(sb, STRBUILDER_ERROR_MEM_ALLOC_FAILED);
}

void strbuilder_finalize(StrBuilder *sb)
{
    if (sb != NULL) {
        mem_free(sb->str);
        sb->str = NULL;
        sb->err = STRBUILDER_ERROR_NONE;
        sb->size = 0;
        sb->len = 0;
    }
}

StrBuilderErr strbuilder_copy(StrBuilder *src, StrBuilder *dest)
{
    StrBuilderErr err = strbuilder_init_sz(dest, MAX(src->len, STRBUILDER_DEFAULT_SIZE));
    if (err == STRBUILDER_ERROR_NONE) {
        memcpy(dest->str, src->str, src->len);
        dest->len = src->len;
    }

    SET_ERROR_RETURN(src, err);
}

const char *strbuilder_get_str(const StrBuilder *sb)
{
    return sb->str;
}

StrBuilderErr strbuilder_get_err(const StrBuilder *sb)
{
    return sb->err;
}

const char *strbuilder_get_error_msg(const StrBuilder *sb)
{
    switch (sb->err) {
        CASE_RETURN_ENUM_STR(STRBUILDER_ERROR_NONE);
        CASE_RETURN_ENUM_STR(STRBUILDER_ERROR_MEM_ALLOC_FAILED);
        CASE_RETURN_ENUM_STR(STRBUILDER_ERROR_INDEX_OUT_OF_BOUNDS);
        default:
            return "Unknown";
    }
}

size_t strbuilder_get_len(const StrBuilder *sb)
{
    return sb->len;
}

StrBuilderErr strbuilder_set_len(StrBuilder *sb, size_t len)
{
    GROW_STR(sb, len);
    if (len > sb->len) {
        char *dst = sb->str + sb->len;
        memset(dst, 0, len - sb->len);
    }

    sb->len = len;
    SET_ERROR_RETURN(sb, STRBUILDER_ERROR_NONE);
}

size_t strbuilder_get_size(const StrBuilder *sb)
{
    return sb->size;
}

StrBuilderErr strbuilder_set_size(StrBuilder *sb, size_t size)
{
    bool success = strbuilder_reallocate_str(sb, size);
    SET_ERROR_RETURN(sb, success ? STRBUILDER_ERROR_NONE : STRBUILDER_ERROR_MEM_ALLOC_FAILED);
}

StrBuilderErr strbuilder_get_char(StrBuilder *sb, int index, char *c)
{
    if (index < 0 || index >= sb->len) {
        *c = '\0';
        SET_ERROR_RETURN(sb, STRBUILDER_ERROR_INDEX_OUT_OF_BOUNDS);
    }

    *c = sb->str[index];
    SET_ERROR_RETURN(sb, STRBUILDER_ERROR_NONE);
}

StrBuilderErr strbuilder_set_char(StrBuilder *sb, int index, char c)
{
    if (index < 0 || index >= sb->len) {
        SET_ERROR_RETURN(sb, STRBUILDER_ERROR_INDEX_OUT_OF_BOUNDS);
    }

    sb->str[index] = c;
    SET_ERROR_RETURN(sb, STRBUILDER_ERROR_NONE);
}

int strbuilder_compare(const StrBuilder *a, const StrBuilder *b)
{
    if (a == b) {
        return 0;
    }

    int result = memcmp(a->str, b->str, MIN(a->len, b->len));
    if (!result) {
        return (int) (a->len - b->len);
    }

    return result;
}

bool strbuilder_equals(const StrBuilder *a, const StrBuilder *b)
{
    return strbuilder_compare(a, b) == 0;
}

bool strbuilder_contains(const StrBuilder *sb, const char *needle, size_t needle_len)
{
    if (needle_len == 0) {
        return true;
    } else if (needle_len == 1) {
        return memchr(sb->str, *needle, sb->len) != NULL;
    } else if (needle_len <= sb->len) {
        char *ptr = sb->str;
        char *end = sb->str + sb->len;
        while (ptr + needle_len < end && (ptr = memchr(ptr, *needle, end - ptr)) != NULL) {
            if (memcmp(ptr, needle, needle_len) == 0) {
                return true;
            }

            ptr += needle_len;
        }
    }

    return false;
}

bool strbuilder_starts_with(const StrBuilder *sb, const char *prefix, size_t prefix_len)
{
    if (sb->len < prefix_len) {
        return false;
    }

    return memcmp(sb->str, prefix, prefix_len) == 0;
}

bool strbuilder_ends_with(const StrBuilder *sb, const char *suffix, size_t suffix_len)
{
    if (sb->len < suffix_len) {
        return false;
    }

    char *ptr = sb->str + sb->len - suffix_len;
    return memcmp(ptr, suffix, suffix_len) == 0;
}

StrBuilderErr strbuilder_append(StrBuilder *sb, const StrBuilder *other)
{
    return strbuilder_append_str(sb, other->str, other->len);
}

StrBuilderErr strbuilder_append_c(StrBuilder *sb, char c)
{
    GROW_STR(sb, sb->len + 1);
    sb->str[sb->len] = c;
    sb->len++;
    SET_ERROR_RETURN(sb, STRBUILDER_ERROR_NONE);
}

StrBuilderErr strbuilder_append_str(StrBuilder *sb, const char *str, size_t len)
{
    size_t newLen = sb->len + len;
    GROW_STR(sb, newLen);
    char *dst = sb->str + sb->len;
    sb->len = newLen;
    memcpy(dst, str, len);
    SET_ERROR_RETURN(sb, STRBUILDER_ERROR_NONE);
}

StrBuilderErr strbuilder_append_i(StrBuilder *sb, int64_t value)
{
    GROW_STR(sb, sb->len + UINT64_MAX_STRLEN);
    char *ptr = sb->str + sb->len;
    int count = snprintf(ptr, sb->size - sb->len, LONG_FMT, value);
    sb->len += count;
    SET_ERROR_RETURN(sb, STRBUILDER_ERROR_NONE);
}

StrBuilderErr strbuilder_append_ui(StrBuilder *sb, uint64_t value)
{
    GROW_STR(sb, sb->len + UINT64_MAX_STRLEN);
    char *ptr = sb->str + sb->len;
    int count = snprintf(ptr, sb->size - sb->len, ULONG_FMT, value);
    sb->len += count;
    SET_ERROR_RETURN(sb, STRBUILDER_ERROR_NONE);
}

StrBuilderErr strbuilder_append_d(StrBuilder *sb, double value)
{
    GROW_STR(sb, sb->len + DOUBLE_MAX_STRLEN);
    char *ptr = sb->str + sb->len;
    int count = snprintf(ptr, sb->size - sb->len, "%f", value);
    sb->len += count;
    SET_ERROR_RETURN(sb, STRBUILDER_ERROR_NONE);
}

int strbuilder_replace_c(StrBuilder *sb, char search, char replace)
{
    int n = 0;
    if (sb->len > 0) {
        char *ptr = sb->str;
        char *end = sb->str + sb->len;
        while ((ptr = memchr(ptr, search, end - ptr)) != NULL) {
            *ptr++ = replace;
            n++;
        }
    }

    return n;
}

static void strbuilder_case_convert(StrBuilder *sb, int (*convert)(int))
{
    if (sb->len > 0) {
        unsigned char *c = (unsigned char *) sb->str;
        unsigned char *e = c + sb->len;

        while (c < e) {
            *c = convert(*c);
            c++;
        }
    }
}

void strbuilder_to_uppercase(StrBuilder *sb)
{
    strbuilder_case_convert(sb, toupper);
}

void strbuilder_to_lowercase(StrBuilder *sb)
{
    strbuilder_case_convert(sb, tolower);
}

StrBuilderErr strbuilder_trim(StrBuilder *sb)
{
    char *start = sb->str;
    char *end = sb->str + sb->len - 1;
    while (isspace(*start) && start <= end) {
        start++;
    }

    while (isspace(*end) && end >= start) {
        end--;
    }

    if (start > end) {
        sb->len = 0; // "   " (3) -> trim -> "" (0)
        SET_ERROR_RETURN(sb, STRBUILDER_ERROR_NONE);
    }

    size_t newLen = end - start + 1;
    if (start > sb->str) {
        memmove(sb->str, start, newLen);
    }

    sb->len = newLen;
    SET_ERROR_RETURN(sb, STRBUILDER_ERROR_NONE);
}

StrBuilderErr strbuilder_repeat(StrBuilder *sb, int times)
{
    if (times < 0) {
        SET_ERROR_RETURN(sb, STRBUILDER_ERROR_INDEX_OUT_OF_BOUNDS);
    } else if (times == 0) {
        sb->len = 0;
    } else if (times > 1) {
        size_t newLen = sb->len + (sb->len * (times - 1));
        GROW_STR(sb, newLen);
        char *dst = sb->str + sb->len;
        while (--times) {
            memmove(dst, sb->str, sb->len);
            dst += sb->len;
        }

        sb->len = newLen;
    }

    SET_ERROR_RETURN(sb, STRBUILDER_ERROR_NONE);
}

void strbuilder_print_debug_info(const StrBuilder *sb)
{
#ifdef DEBUG
    printf("StrBuilder@%p {\n"
           "    length             : %zu\n"
           "    allocated memory   : %zu bytes\n"
           "    unused memory      : %zu bytes (%zu%%)\n"
           "    last error code    : %d\n"
           "    last error message : %s\n"
           "    string             : \"%.*s\"\n"
           "}",
           sb,
           sb->len,
           sb->size,
           sb->size - sb->len,
           100 - (sb->len * 100 / sb->size),
           sb->err,
           strbuilder_get_error_msg(sb),
           (int) sb->len,
           sb->str);
#endif
}
