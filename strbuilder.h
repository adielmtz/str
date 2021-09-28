#ifndef STRBUILDER_STRBUILDER_H
#define STRBUILDER_STRBUILDER_H

#include <stdlib.h>

#define STRBUILDER_DEFAULT_SIZE 16

typedef enum StrBuilderErr
{
    STRBUILDER_SUCCESS,
    STRBUILDER_ERROR,
    STRBUILDER_NOT_IMPLEMENTED,
    STRBUILDER_MEM_ALLOC_FAILED,
    STRBUILDER_INDEX_OUT_OF_BOUNDS,
} StrBuilderErr;

typedef struct StrBuilder StrBuilder;

const char *strbuilder_get_error_str(StrBuilderErr err);

StrBuilderErr strbuilder_create(StrBuilder **result);
StrBuilderErr strbuilder_create_sz(StrBuilder **result, size_t size);
void strbuilder_free(StrBuilder *sb);

StrBuilderErr strbuilder_append(StrBuilder *sb, const StrBuilder *other);
StrBuilderErr strbuilder_append_c(StrBuilder *sb, char c);
StrBuilderErr strbuilder_append_str(StrBuilder *sb, const char *str, size_t len);

#endif // STRBUILDER_STRBUILDER_H
