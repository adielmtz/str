#ifndef STRBUILDER_STRBUILDER_H
#define STRBUILDER_STRBUILDER_H

#include <stdlib.h>
#include <stdint.h>

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

size_t strbuilder_get_len(const StrBuilder *sb);
StrBuilderErr strbuilder_set_len(StrBuilder *sb, size_t len);
size_t strbuilder_get_size(const StrBuilder *sb);
StrBuilderErr strbuilder_set_size(StrBuilder *sb, size_t size);

char *strbuilder_to_cstr(const StrBuilder *sb);
StrBuilderErr strbuilder_append(StrBuilder *sb, const StrBuilder *other);
StrBuilderErr strbuilder_append_c(StrBuilder *sb, char c);
StrBuilderErr strbuilder_append_str(StrBuilder *sb, const char *str, size_t len);
StrBuilderErr strbuilder_append_i(StrBuilder *sb, int64_t value);
StrBuilderErr strbuilder_append_ui(StrBuilder *sb, uint64_t value);

#endif // STRBUILDER_STRBUILDER_H
