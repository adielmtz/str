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
#   define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef MAX
#   define MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif

#define SET_ERROR_RETURN(sb, value) do { \
    (sb)->err = (value);                 \
    return (value);                      \
} while (0)

#define MEMBLOCK_COPY_OR_EXPAND(sb, requiredSize) do {                             \
    if (!strbuilder_reallocate_refcounted_memblock((sb), (requiredSize), false)) { \
        SET_ERROR_RETURN((sb), STRBUILDER_ERROR_MEM_ALLOC_FAILED);                 \
    }                                                                              \
} while (0)

static void *(*mem_allocate)(size_t) = malloc;

static void *(*mem_reallocate)(void *, size_t) = realloc;

static void (*mem_free)(void *) = free;

struct MemBlock
{
    int64_t refcount;
    size_t size;
    char mem[];
};

struct StrBuilder
{
    StrBuilderErr err;
    size_t len;
    char *str;
    struct MemBlock *block;
};

static bool strbuilder_memblock_copy(StrBuilder *sb, size_t blockSize)
{
    struct MemBlock *block = mem_allocate(sizeof(struct MemBlock) + sizeof(char) * blockSize);
    if (block != NULL) {
        block->refcount = 1;
        block->size = blockSize;
        memcpy(block->mem, sb->str, MIN(blockSize, sb->len));

        // Remove this reference
        assert(sb->block->refcount > 1);
        sb->block->refcount--;
        sb->str = block->mem;
        sb->block = block;
        return true;
    }

    return false;
}

static bool strbuilder_memblock_reallocate(StrBuilder *sb, size_t newSize)
{
    struct MemBlock *tmp = mem_reallocate(sb->block, sizeof(struct MemBlock) + sizeof(char) * newSize);
    if (tmp != NULL) {
        tmp->size = newSize;
        sb->str = tmp->mem;
        sb->block = tmp;
        return true;
    }

    return false;
}

static bool strbuilder_reallocate_refcounted_memblock(StrBuilder *sb, size_t requiredSize, bool allow_shrink)
{
    size_t newSize = requiredSize;
    if (requiredSize > sb->block->size) {
        newSize = sb->block->size * 2;
        if (requiredSize > newSize) {
            newSize = requiredSize;
        }
    }

    if (sb->block->refcount > 1) {
        // This block is referenced by more than 1 instance of StrBuilder.
        // We need to create a copy of it and use it for this particular StrBuilder object.
        if (!strbuilder_memblock_copy(sb, newSize)) {
            return false;
        }
    } else if (newSize > sb->block->size || allow_shrink) {
        // This block has a refcount of 1, meaning that is not referenced anywhere else,
        // so we can safely reallocate it without having to update its refcount.
        if (!strbuilder_memblock_reallocate(sb, newSize)) {
            return false;
        }
    }

    if (sb->len > newSize) {
        sb->len = newSize;
    }

    return true;
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

StrBuilderErr strbuilder_create(StrBuilder **result)
{
    return strbuilder_create_sz(result, STRBUILDER_DEFAULT_SIZE);
}

StrBuilderErr strbuilder_create_sz(StrBuilder **result, size_t size)
{
    StrBuilder *sb = mem_allocate(sizeof(StrBuilder));
    *result = NULL;

    if (sb != NULL) {
        struct MemBlock *block = mem_allocate(sizeof(struct MemBlock) + sizeof(char) * size);

        if (block != NULL) {
            sb->len = 0;
            sb->str = block->mem;
            sb->block = block;
            block->size = size;
            block->refcount = 1;
            *result = sb;
            SET_ERROR_RETURN(sb, STRBUILDER_ERROR_NONE);
        } else {
            mem_free(sb);
        }
    }

    return STRBUILDER_ERROR_MEM_ALLOC_FAILED;
}

void strbuilder_free(StrBuilder *sb)
{
    if (sb != NULL) {
        sb->block->refcount--;
        if (sb->block->refcount == 0) {
            mem_free(sb->block);
        }

        mem_free(sb);
    }
}

StrBuilderErr strbuilder_copy(StrBuilder *sb, StrBuilder **result)
{
    StrBuilder *copy = mem_allocate(sizeof(StrBuilder));
    *result = NULL;

    if (copy != NULL) {
        // Copy StrBuilder properties
        copy->err = STRBUILDER_ERROR_NONE;
        copy->len = sb->len;
        copy->str = sb->str;
        copy->block = sb->block;
        copy->block->refcount++;

        *result = copy;
        SET_ERROR_RETURN(sb, STRBUILDER_ERROR_NONE);
    }

    return STRBUILDER_ERROR_MEM_ALLOC_FAILED;
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
    size_t oldLen = sb->len;
    MEMBLOCK_COPY_OR_EXPAND(sb, len);
    if (len > oldLen) {
        char *dst = sb->str + sb->len;
        memset(dst, '\0', len - oldLen);
    }

    sb->len = len;
    SET_ERROR_RETURN(sb, STRBUILDER_ERROR_NONE);
}

size_t strbuilder_get_size(const StrBuilder *sb)
{
    return sb->block->size;
}

StrBuilderErr strbuilder_set_size(StrBuilder *sb, size_t size)
{
    bool success = strbuilder_reallocate_refcounted_memblock(sb, size, true);
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

    // We need to make a copy of the memblock if it has a refcount > 1
    MEMBLOCK_COPY_OR_EXPAND(sb, sb->len);
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
    MEMBLOCK_COPY_OR_EXPAND(sb, sb->len + 2);
    sb->str[sb->len] = c;
    sb->str[sb->len + 1] = '\0';
    sb->len++;
    SET_ERROR_RETURN(sb, STRBUILDER_ERROR_NONE);
}

StrBuilderErr strbuilder_append_str(StrBuilder *sb, const char *str, size_t len)
{
    size_t newLen = sb->len + len;
    MEMBLOCK_COPY_OR_EXPAND(sb, newLen + 1);
    char *dst = sb->str + sb->len;
    sb->len = newLen;
    memcpy(dst, str, len);
    sb->str[sb->len] = '\0';
    SET_ERROR_RETURN(sb, STRBUILDER_ERROR_NONE);
}

StrBuilderErr strbuilder_append_i(StrBuilder *sb, int64_t value)
{
    MEMBLOCK_COPY_OR_EXPAND(sb, sb->len + UINT64_MAX_STRLEN);
    char *ptr = sb->str + sb->len;
    int count = snprintf(ptr, sb->block->size - sb->len, LONG_FMT, value);
    sb->len += count;
    SET_ERROR_RETURN(sb, STRBUILDER_ERROR_NONE);
}

StrBuilderErr strbuilder_append_ui(StrBuilder *sb, uint64_t value)
{
    MEMBLOCK_COPY_OR_EXPAND(sb, sb->len + UINT64_MAX_STRLEN);
    char *ptr = sb->str + sb->len;
    int count = snprintf(ptr, sb->block->size - sb->len, ULONG_FMT, value);
    sb->len += count;
    SET_ERROR_RETURN(sb, STRBUILDER_ERROR_NONE);
}

StrBuilderErr strbuilder_append_d(StrBuilder *sb, double value)
{
    MEMBLOCK_COPY_OR_EXPAND(sb, sb->len + DOUBLE_MAX_STRLEN);
    char *ptr = sb->str + sb->len;
    int count = snprintf(ptr, sb->block->size - sb->len, "%f", value);
    sb->len += count;
    SET_ERROR_RETURN(sb, STRBUILDER_ERROR_NONE);
}

StrBuilderErr strbuilder_replace_c(StrBuilder *sb, char search, char replace, int *count)
{
    MEMBLOCK_COPY_OR_EXPAND(sb, sb->len);
    int n = 0;
    if (sb->len > 0) {
        char *ptr = sb->str;
        char *end = sb->str + sb->len;
        while ((ptr = memchr(ptr, search, end - ptr)) != NULL) {
            *ptr++ = replace;
            n++;
        }
    }

    if (count != NULL) {
        *count = n;
    }

    SET_ERROR_RETURN(sb, STRBUILDER_ERROR_NONE);
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

StrBuilderErr strbuilder_to_uppercase(StrBuilder *sb)
{
    MEMBLOCK_COPY_OR_EXPAND(sb, sb->len);
    strbuilder_case_convert(sb, toupper);
    SET_ERROR_RETURN(sb, STRBUILDER_ERROR_NONE);
}

StrBuilderErr strbuilder_to_lowercase(StrBuilder *sb)
{
    MEMBLOCK_COPY_OR_EXPAND(sb, sb->len);
    strbuilder_case_convert(sb, tolower);
    SET_ERROR_RETURN(sb, STRBUILDER_ERROR_NONE);
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
        sb->str[sb->len] = '\0';
        SET_ERROR_RETURN(sb, STRBUILDER_ERROR_NONE);
    }

    size_t newLen = end - start + 1;
    if (start > sb->str) {
        if (sb->block->refcount > 1) {
            // This string has more references,
            // so instead of reallocating, we can
            // set the pointer to the start of the
            // "trimmed" substring.
            sb->str = start;
        } else {
            // In this case, the string has only 1 ref,
            // we can modify its contents
            memmove(sb->str, start, newLen);
        }
    }

    sb->len = newLen;
    sb->str[sb->len] = '\0';
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
        MEMBLOCK_COPY_OR_EXPAND(sb, newLen);
        char *dst = sb->str + sb->len;
        while (--times) {
            memmove(dst, sb->str, sb->len);
            dst += sb->len;
        }

        sb->len = newLen;
    }

    sb->str[sb->len] = '\0';
    SET_ERROR_RETURN(sb, STRBUILDER_ERROR_NONE);
}

void strbuilder_print_debug_info(const StrBuilder *sb)
{
#ifdef DEBUG
    printf("StrBuilder@%p {\n"
           "    last error code    : %d\n"
           "    last error message : %s\n"
           "    length             : %zu\n"
           "    string             : \"%*s\"\n"
           "    MemBlock@%p {\n"
           "        refcount         : %zu\n"
           "        allocated memory : %zu bytes\n"
           "        unused memory    : %zu bytes (%zu%%)\n"
           "    }\n"
           "}\n",
           sb,
           sb->err,
           strbuilder_get_error_msg(sb),
           sb->len,
           (int) sb->len,
           sb->str,
           sb->block,
           sb->block->refcount,
           sb->block->size,
           sb->block->size - sb->len,
           100 - (sb->len * 100 / sb->block->size)
    );
#endif
}
