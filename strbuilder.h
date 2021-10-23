#ifndef STRBUILDER_STRBUILDER_H
#define STRBUILDER_STRBUILDER_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define STRBUILDER_DEFAULT_SIZE 16

typedef enum StrBuilderErr
{
    STRBUILDER_ERROR_NONE,
    STRBUILDER_ERROR_MEM_ALLOC_FAILED,
    STRBUILDER_ERROR_INDEX_OUT_OF_BOUNDS,
} StrBuilderErr;

typedef struct StrBuilder StrBuilder;

StrBuilderErr strbuilder_create(StrBuilder **result);
StrBuilderErr strbuilder_create_sz(StrBuilder **result, size_t size);
void strbuilder_free(StrBuilder *sb);

StrBuilderErr strbuilder_get_err(const StrBuilder *sb);
const char *strbuilder_get_error_msg(const StrBuilder *sb);

size_t strbuilder_get_len(const StrBuilder *sb);
StrBuilderErr strbuilder_set_len(StrBuilder *sb, size_t len);
size_t strbuilder_get_size(const StrBuilder *sb);
StrBuilderErr strbuilder_set_size(StrBuilder *sb, size_t size);

StrBuilderErr strbuilder_get_char(StrBuilder *sb, size_t index, char *c);
StrBuilderErr strbuilder_set_char(StrBuilder *sb, size_t index, char c);

int strbuilder_compare(const StrBuilder *a, const StrBuilder *b);
bool strbuilder_equals(const StrBuilder *a, const StrBuilder *b);

char *strbuilder_to_cstr(const StrBuilder *sb);
const char *strbuilder_get_cstr(const StrBuilder *sb);
StrBuilderErr strbuilder_copy(StrBuilder *sb, StrBuilder **result);
StrBuilderErr strbuilder_append(StrBuilder *sb, const StrBuilder *other);
StrBuilderErr strbuilder_append_c(StrBuilder *sb, char c);
StrBuilderErr strbuilder_append_str(StrBuilder *sb, const char *str, size_t len);
StrBuilderErr strbuilder_append_i(StrBuilder *sb, int64_t value);
StrBuilderErr strbuilder_append_ui(StrBuilder *sb, uint64_t value);

StrBuilderErr strbuilder_repeat(StrBuilder *sb, int times);
bool strbuilder_starts_with(const StrBuilder *sb, const char *prefix, size_t prefix_len);
bool strbuilder_ends_with(const StrBuilder *sb, const char *suffix, size_t suffix_len);

#endif // STRBUILDER_STRBUILDER_H
