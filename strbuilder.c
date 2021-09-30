#include "strbuilder.h"

#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <inttypes.h>

#define MAX_UINT64_LEN 21
#define CASE_RETURN_ENUM_STR(val) case val: return #val
#define EMPTY_DEFAULT_SWITCH_CASE() default: assert(0)
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

struct StrBuilder
{
    size_t size;
    size_t len;
    char *str;
};

static bool strbuilder_reallocate_str(StrBuilder *sb, size_t newSize)
{
    char *tmp = realloc(sb->str, sizeof(char) * newSize);
    if (tmp != NULL) {
        sb->size = newSize;
        sb->str = tmp;
        if (sb->len > newSize) {
            sb->len = newSize;
        }

        return true;
    }

    return false;
}

static bool strbuilder_grow_str(StrBuilder *sb, size_t requiredSize)
{
    if (requiredSize > sb->size) {
        size_t newSize = sb->size * 2;
        if (requiredSize > newSize) {
            newSize = requiredSize;
        }

        return strbuilder_reallocate_str(sb, newSize);
    }

    // The operation succeeded as there was no need to reallocate
    return true;
}

const char *strbuilder_get_error_str(StrBuilderErr err)
{
    switch (err) {
        CASE_RETURN_ENUM_STR(STRBUILDER_SUCCESS);
        CASE_RETURN_ENUM_STR(STRBUILDER_ERROR);
        CASE_RETURN_ENUM_STR(STRBUILDER_NOT_IMPLEMENTED);
        CASE_RETURN_ENUM_STR(STRBUILDER_MEM_ALLOC_FAILED);
        CASE_RETURN_ENUM_STR(STRBUILDER_INDEX_OUT_OF_BOUNDS);
        EMPTY_DEFAULT_SWITCH_CASE();
    }
}

StrBuilderErr strbuilder_create(StrBuilder **result)
{
    return strbuilder_create_sz(result, STRBUILDER_DEFAULT_SIZE);
}

StrBuilderErr strbuilder_create_sz(StrBuilder **result, size_t size)
{
    StrBuilder *sb = malloc(sizeof(StrBuilder));
    if (sb == NULL) {
        *result = NULL;
        return STRBUILDER_MEM_ALLOC_FAILED;
    }

    sb->str = calloc(size, sizeof(char));
    if (sb->str == NULL) {
        free(sb);
        *result = NULL;
        return STRBUILDER_MEM_ALLOC_FAILED;
    }

    // Everything is OK!
    sb->size = size;
    sb->len = 0;
    *result = sb;
    return STRBUILDER_SUCCESS;
}

void strbuilder_free(StrBuilder *sb)
{
    if (sb != NULL) {
        free(sb->str);
        free(sb);
    }
}

size_t strbuilder_get_len(const StrBuilder *sb)
{
    return sb->len;
}

StrBuilderErr strbuilder_set_len(StrBuilder *sb, size_t len)
{
    if (len > sb->size && !strbuilder_reallocate_str(sb, len)) {
        return STRBUILDER_MEM_ALLOC_FAILED;
    }

    if (len > sb->len) {
        char *dst = sb->str + sb->len;
        memset(dst, '\0', len - sb->len);
    }

    sb->len = len;
    return STRBUILDER_SUCCESS;
}

size_t strbuilder_get_size(const StrBuilder *sb)
{
    return sb->size;
}

StrBuilderErr strbuilder_set_size(StrBuilder *sb, size_t size)
{
    if (!strbuilder_reallocate_str(sb, size)) {
        return STRBUILDER_MEM_ALLOC_FAILED;
    }

    return STRBUILDER_SUCCESS;
}

StrBuilderErr strbuilder_get_char(const StrBuilder *sb, size_t index, char *c)
{
    if (index > sb->len) {
        *c = '\0';
        return STRBUILDER_INDEX_OUT_OF_BOUNDS;
    }

    *c = sb->str[index];
    return STRBUILDER_SUCCESS;
}

StrBuilderErr strbuilder_set_char(StrBuilder *sb, size_t index, char c)
{
    if (index > sb->len) {
        return STRBUILDER_INDEX_OUT_OF_BOUNDS;
    }

    sb->str[index] = c;
    return STRBUILDER_SUCCESS;
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

char *strbuilder_to_cstr(const StrBuilder *sb)
{
    char *result = malloc(sizeof(char) * sb->len + 1);
    if (result != NULL) {
        memcpy(result, sb->str, sb->len);
        result[sb->len] = '\0';
    }

    return result;
}

StrBuilderErr strbuilder_append(StrBuilder *sb, const StrBuilder *other)
{
    return strbuilder_append_str(sb, other->str, other->len);
}

StrBuilderErr strbuilder_append_c(StrBuilder *sb, char c)
{
    return strbuilder_append_str(sb, &c, 1);
}

StrBuilderErr strbuilder_append_str(StrBuilder *sb, const char *str, size_t len)
{
    size_t newLen = sb->len + len;
    if (!strbuilder_grow_str(sb, newLen)) {
        return STRBUILDER_MEM_ALLOC_FAILED;
    }

    char *dst = sb->str + sb->len;
    sb->len = newLen;
    memcpy(dst, str, len);
    return STRBUILDER_SUCCESS;
}

StrBuilderErr strbuilder_append_i(StrBuilder *sb, int64_t value)
{
    if (!strbuilder_grow_str(sb, sb->len + MAX_UINT64_LEN)) {
        return STRBUILDER_MEM_ALLOC_FAILED;
    }

    char buff[MAX_UINT64_LEN];
    int count = snprintf(buff, sizeof(buff), "%" PRId64, value);
    char *dst = sb->str + sb->len;
    memcpy(dst, buff, count);
    sb->len += count;
    return STRBUILDER_SUCCESS;
}

StrBuilderErr strbuilder_append_ui(StrBuilder *sb, uint64_t value)
{
    if (!strbuilder_grow_str(sb, sb->len + MAX_UINT64_LEN)) {
        return STRBUILDER_MEM_ALLOC_FAILED;
    }

    char buff[MAX_UINT64_LEN];
    int count = snprintf(buff, sizeof(buff), "%" PRIu64, value);
    char *dst = sb->str + sb->len;
    memcpy(dst, buff, count);
    sb->len += count;
    return STRBUILDER_SUCCESS;
}
