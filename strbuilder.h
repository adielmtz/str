#ifndef STRBUILDER_STRBUILDER_H
#define STRBUILDER_STRBUILDER_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define STRBUILDER_DEFAULT_SIZE 16

typedef enum StrBuilderErr
{
    STRBUILDER_ERROR_NONE,
    STRBUILDER_ERROR_NULL_POINTER,
    STRBUILDER_ERROR_MEM_ALLOC_FAILED,
    STRBUILDER_ERROR_INDEX_OUT_OF_BOUNDS,
} StrBuilderErr;

struct MemBlock;

typedef struct StrBuilder
{
    StrBuilderErr err;
    size_t len;
    char *str;
    struct MemBlock *block;
} StrBuilder;

/**
 * Sets the memory allocator that the StrBuilder API will use.
 * Defaults to malloc() from stdlib.h
 *
 * @param mem_alloc_fn
 */
void strbuilder_set_mem_allocator(void *(*mem_alloc_fn)(size_t));

/**
 * Sets the memory reallocator that the StrBuilder API will use.
 * Defaults to realloc() from stdlib.h
 *
 * @param mem_realloc_fn
 */
void strbuilder_set_mem_reallocator(void *(*mem_realloc_fn)(void *, size_t));

/**
 * Sets the memory deallocator that the StrBuilder API will use.
 * Defaults to free() from stdlib.h
 *
 * @param mem_free_fn
 */
void strbuilder_set_mem_free(void (*mem_free_fn)(void *));

/**
 * Initializes a StrBuilder object using the default memory size.
 *
 * @param result The StrBuilder handle.
 *
 * @return STRBUILDER_ERROR_NONE on success. On failure, the result param
 * will be set to NULL.
 */
StrBuilderErr strbuilder_init(StrBuilder *sb);

/**
 * Initializes a StrBuilder object.
 *
 * @param sb The StrBuilder handle.
 * @param size The amount of memory to allocate.
 *
 * @return STRBUILDER_ERROR_NONE on success. On failure, the sb param
 * will be set to NULL.
 */
StrBuilderErr strbuilder_init_size(StrBuilder *sb, size_t size);

/**
 * Releases the memory used by the StrBuilder and deallocates the handle.
 *
 * @param sb The StrBuilder handle to deallocate.
 */
void strbuilder_finalize(StrBuilder *sb);

/**
 * Copies the string into a new StrBuilder object.
 *
 * @param sb The StrBuilder handle to copy from.
 * @param result The result StrBuilder handle.
 *
 * @return STRBUILDER_ERROR_NONE on success. On failure, the result param
 * will be set to NULL.
 */
StrBuilderErr strbuilder_copy(StrBuilder *sb, StrBuilder *result);

/**
 * Gets the internal string pointer used by the StrBuilder.
 * The caller must not free or reallocate this value and it should be treated as read-only memory.
 *
 * @param sb The StrBuilder handle.
 *
 * @return
 */
const char *strbuilder_get_str(const StrBuilder *sb);

/**
 * Gets the last occurred error code.
 *
 * @param sb The StrBuilder handle.
 *
 * @return
 */
StrBuilderErr strbuilder_get_err(const StrBuilder *sb);

/**
 * Gets the error string of the last occurred error.
 *
 * @param sb The StrBuilder handle.
 *
 * @return
 */
const char *strbuilder_get_error_msg(const StrBuilder *sb);

/**
 * Gets the length of the string.
 *
 * @param sb The StrBuilder handle.
 *
 * @return
 */
size_t strbuilder_get_len(const StrBuilder *sb);

/**
 * Sets the length of the string.
 *
 * @param sb The StrBuilder handle.
 * @param len The length of the string.
 * <br>If the length is shorter, the string is truncated.
 * <br>If the length is larger, the gap will be padded with NULL characters.
 * The API will reallocate more memory (if necessary) to satisfy the requested length.
 *
 * @return STRBUILDER_ERROR_NONE on success.
 */
StrBuilderErr strbuilder_set_len(StrBuilder *sb, size_t len);

/**
 * Gets the amount of memory allocated for the StrBuilder.
 * This value is equal to or greater than the length of the string.
 *
 * @param sb The StrBuilder handle.
 */
size_t strbuilder_get_size(const StrBuilder *sb);

/**
 * Sets the amount of memory allocated for the StrBuilder.
 *
 * @param sb The StrBuilder handle.
 * @param size The new amount of memory to allocate.
 * <br>If the size is less than the length, the string will be truncated.
 *
 * @return STRBUILDER_ERROR_NONE on success.
 */
StrBuilderErr strbuilder_set_size(StrBuilder *sb, size_t size);

/**
 * Get the character found at the specified position in the string.
 *
 * @param sb The StrBuilder handle.
 * @param index The index of the character.
 * @param c Out: the char pointer.
 *
 * @return STRBUILDER_ERROR_NONE on success.
 */
StrBuilderErr strbuilder_get_char(StrBuilder *sb, int index, char *c);

/**
 * Replaces the character at the specified position in the string.
 *
 * @param sb The StrBuilder handle.
 * @param index The index of the character to replace.
 * @param c The character to set.
 *
 * @return STRBUILDER_ERROR_NONE on success.
 */
StrBuilderErr strbuilder_set_char(StrBuilder *sb, int index, char c);

/**
 * Compares 2 strings.
 *
 * @param a The first StrBuilder.
 * @param b The second StrBuilder.
 *
 * @return Returns a negative integer if the first string is less than the second string.
 * <br>Returns a positive integer if the first string is greater than the second string.
 * <br>Returns zero if both strings are equal.
 */
int strbuilder_compare(const StrBuilder *a, const StrBuilder *b);

/**
 * Returns true if 2 strings objects are equal.
 *
 * @param a The first StrBuilder.
 * @param b The second StrBuilder.
 *
 * @return
 */
bool strbuilder_equals(const StrBuilder *a, const StrBuilder *b);

/**
 * Returns true if the string contains the given substring.
 *
 * @param sb The StrBuilder handle.
 * @param needle The substring to search for.
 * @param needle_len The length of the substring.
 *
 * @return Returns true if the substring is in the StrBuilder.
 * An empty needle (needle_len = 0) will always return true.
 */
bool strbuilder_contains(const StrBuilder *sb, const char *needle, size_t needle_len);

/**
 * Returns true if the string starts with the given substring.
 *
 * @param sb The StrBuilder handle.
 * @param prefix The substring to search for.
 * @param prefix_len The length of the substring.
 *
 * @return Returns true if the StrBuilder begins with the substring.
 * An empty needle (needle_len = 0) will always return true.
 */
bool strbuilder_starts_with(const StrBuilder *sb, const char *prefix, size_t prefix_len);

/**
 * Returns true if the string ends with the given substring.
 *
 * @param sb The StrBuilder handle.
 * @param suffix The substring to search for.
 * @param suffix_len The length of the substring.
 *
 * @return Returns true if the StrBuilder ends with the substring.
 * An empty needle (needle_len = 0) will always return true.
 */
bool strbuilder_ends_with(const StrBuilder *sb, const char *suffix, size_t suffix_len);

/**
 * Appends a StrBuilder value to the string.
 *
 * @param sb The StrBuilder handle.
 * @param other The StrBuilder handle to append.
 *
 * @return STRBUILDER_ERROR_NONE on success.
 */
StrBuilderErr strbuilder_append(StrBuilder *sb, const StrBuilder *other);

/**
 * Appends a character to the string.
 *
 * @param sb The StrBuilder handle.
 * @param c The character to append.
 *
 * @return STRBUILDER_ERROR_NONE on success.
 */
StrBuilderErr strbuilder_append_c(StrBuilder *sb, char c);

/**
 * Appends a string to the string.
 *
 * @param sb The StrBuilder handle.
 * @param str The string to append.
 * @param len The length of the string.
 *
 * @return STRBUILDER_ERROR_NONE on success.
 */
StrBuilderErr strbuilder_append_str(StrBuilder *sb, const char *str, size_t len);

/**
 * Appends a signed integer to the string.
 *
 * @param sb The StrBuilder handle.
 * @param value The signed integer to append.
 *
 * @return STRBUILDER_ERROR_NONE on success.
 */
StrBuilderErr strbuilder_append_i(StrBuilder *sb, int64_t value);

/**
 * Appends an unsigned integer to the string.
 *
 * @param sb The StrBuilder handle.
 * @param value The unsigned integer to append.
 *
 * @return STRBUILDER_ERROR_NONE on success.
 */
StrBuilderErr strbuilder_append_ui(StrBuilder *sb, uint64_t value);

/**
 * Appends a floating point value to the string.
 *
 * @param sb The StrBuilder handle.
 * @param value The floating point value to append.
 *
 * @return STRBUILDER_ERROR_NONE on success.
 */
StrBuilderErr strbuilder_append_d(StrBuilder *sb, double value);

/**
 * Replaces all occurrences of a character with a different character.
 *
 * @param sb The StrBuilder handle.
 * @param search The character to search.
 * @param replace The character to replace with.
 * @param count If given, the function will set the number of replacements made.
 *
 * @return
 */
StrBuilderErr strbuilder_replace_c(StrBuilder *sb, char search, char replace, int *count);

/**
 * Converts the string to uppercase.
 *
 * @param sb The StrBuilder handle.
 *
 * @return STRBUILDER_ERROR_NONE on success.
 */
StrBuilderErr strbuilder_to_uppercase(StrBuilder *sb);

/**
 * Converts the string to lowercase.
 *
 * @param sb The StrBuilder handle.
 *
 * @return STRBUILDER_ERROR_NONE on success.
 */
StrBuilderErr strbuilder_to_lowercase(StrBuilder *sb);

/**
 * Strip whitespace from the beginning and end of the StrBuilder.
 *
 * @param sb The StrBuilder handle.
 *
 * @return STRBUILDER_ERROR_NONE on success.
 */
StrBuilderErr strbuilder_trim(StrBuilder *sb);

/**
 * Repeats the string.
 *
 * @param sb The StrBuilder handle.
 * @param times The number of times that the string should be repeated.
 * This value has to be equal or greater than zero. If the number of times
 * is 0, the string will be truncated to len=0.
 *
 * @return STRBUILDER_ERROR_NONE on success.
 */
StrBuilderErr strbuilder_repeat(StrBuilder *sb, int times);

/**
 * Prints debug info about the given StrBuilder object.
 * Only available when DEBUG symbol is defined at compile time.
 *
 * @param sb The StrBuilder handle.
 */
void strbuilder_print_debug_info(const StrBuilder *sb);

#endif // STRBUILDER_STRBUILDER_H
