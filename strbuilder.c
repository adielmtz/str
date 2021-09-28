#include "strbuilder.h"

#include <assert.h>

#define CASE_RETURN_ENUM_STR(val) case val: return #val
#define EMPTY_DEFAULT_SWITCH_CASE() default: assert(0)

struct StrBuilder
{
    size_t size;
    size_t len;
    char *str;
};

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
