#pragma once

#include <stdint.h>
#include <stdbool.h>

#define MUTSTR_DEFAULT_INITIAL_SIZE 16

typedef enum MutStrState
{
    MUTSTR_ERROR_NONE,
    MUTSTR_ERROR_MEM_ALLOC_FAILURE,
    MUTSTR_ERROR_OUT_OF_RANGE,
} MutStrState;

typedef struct MutStr
{
    char *value;
    int32_t length;
    int32_t size;
    MutStrState error;
} MutStr;

/**
 * Sets a custom memory allocator that the MutStr API will use.
 *
 * @param malloc_fn
 */
void mutstr_set_memory_alloc(void *(*malloc_fn)(size_t));

/**
 * Sets a custom memory reallocator that the MutStr API will use.
 *
 * @param realloc_fn
 */
void mutstr_set_memory_realloc(void *(*realloc_fn)(void *, size_t));

/**
 * Sets a custom memory deallocator (free) that the MutStr API will use.
 *
 * @param free_fn
 */
void mutstr_set_memory_free(void (*free_fn)(void *));

/**
 * Gets the error string for the given error code.
 *
 * @param code The error code.
 *
 * @return A const char pointer.
 */
const char *mutstr_get_error_msg(MutStrState code);

/**
 * Initializes a MutStr object.
 *
 * @param sb The MutStr handle.
 *
 * @return MUTSTR_ERROR_NONE on success.
 */
MutStrState mutstr_init(MutStr *sb);

/**
 * Initializes a MutStr object with a custom memory size.
 *
 * @param sb The MutStr handle.
 * @param size The amount of memory to allocate.
 *
 * @return MUTSTR_ERROR_NONE on success.
 */
MutStrState mutstr_init_size(MutStr *sb, int32_t size);

/**
 * Releases the memory used by the MutStr object.
 *
 * @param sb The MutStr handle.
 */
void mutstr_finalize(MutStr *sb);

/**
 * Copies the string into a different MutStr object.
 *
 * @param src The source MutStr handle.
 * @param dest The destination MutStr handle. This object must not be initialized
 * or else a memory leak may occur.
 *
 * @return MUTSTR_ERROR_NONE on success.
 */
MutStrState mutstr_copy(MutStr *src, MutStr *dest);

/**
 * Sets the length of the string.
 *
 * @param sb The MutStr handle.
 * @param len The length of the string.
 * <br>If the length is shorter, the string is truncated.
 * <br>If the length is larger, the gap will be padded with NULL characters.
 * The API will reallocate more memory (if necessary) to satisfy the requested length.
 *
 * @return MUTSTR_ERROR_NONE on success.
 */
MutStrState mutstr_set_length(MutStr *sb, int32_t length);

/**
 * Sets the amount of memory allocated for the MutStr object.
 *
 * @param sb The MutStr handle.
 * @param size The new amount of memory to reallocate.
 * <br>If the size is less than the length, the string will be truncated.
 *
 * @return MUTSTR_ERROR_NONE on success.
 */
MutStrState mutstr_set_size(MutStr *sb, int32_t newSize);

/**
 * Compares 2 strings.
 *
 * @param a The first MutStr handle.
 * @param b The second MutStr handle.
 *
 * @return Returns a negative integer if the first string is less than the second string.
 * <br>Returns a positive integer if the first string is greater than the second string.
 * <br>Returns zero if both strings are equal.
 */
int mutstr_compare(const MutStr *a, const MutStr *b);

/**
 * Returns true if both strings are equal.
 *
 * @param a The first MutStr handle.
 * @param b The second MutStr handle.
 */
bool mutstr_equals(const MutStr *a, const MutStr *b);

/**
 * Find the position of the first occurrence of a substring in a string.
 *
 * @param sb The MutStr handle.
 * @param needle The substring to search for.
 * @param needle_len The length of the substring.
 *
 * @return The zero-based index in the string or -1 if the substring was not found.
 */
int32_t mutstr_indexof(const MutStr *sb, const char *needle, int32_t needle_len);

/**
 * Returns true if the string contains the given substring.
 *
 * @param sb The MutStr handle.
 * @param needle The substring to search for.
 * @param needle_len The length of the substring.
 *
 * @return Returns true if the substring is found in the string.
 * An empty needle (needle_len=0) always returns true.
 */
bool mutstr_contains(const MutStr *sb, const char *needle, int32_t needle_len);

/**
 * Returns true if the string starts with the given substring.
 *
 * @param sb The MutStr handle.
 * @param prefix The substring to search for.
 * @param prefix_len The length of the substring.
 *
 * @return Returns true if the string begins with the substring.
 * An empty needle (needle_len=0) always returns true.
 */
bool mutstr_starts_with(const MutStr *sb, const char *prefix, int32_t prefix_len);

/**
 * Returns true if the string ends with the given substring.
 *
 * @param sb The MutStr handle.
 * @param suffix The substring to search for.
 * @param suffix_len The length of the substring.
 *
 * @return Returns true if the string ends with the substring.
 * An empty needle (needle_len=0) always returns true.
 */
bool mutstr_ends_with(const MutStr *sb, const char *suffix, int32_t suffix_len);

/**
 * Concatenates the string with another MutStr object.
 *
 * @param sb The MutStr handle.
 * @param other The MutStr handle to append.
 *
 * @return MUTSTR_ERROR_NONE on success.
 */
MutStrState mutstr_concat(MutStr *sb, const MutStr *other);

/**
 * Appends a character.
 *
 * @param sb The MutStr handle.
 * @param c The character to append.
 *
 * @return MUTSTR_ERROR_NONE on success.
 */
MutStrState mutstr_append_char(MutStr *sb, char c);

/**
 * Appends a string.
 *
 * @param sb The MutStr handle.
 * @param str The string to append.
 * @param len The length of the string.
 *
 * @return MUTSTR_ERROR_NONE on success.
 */
MutStrState mutstr_append_string(MutStr *sb, const char *string, int32_t len);

/**
 * Appends a formatted string.
 *
 * @param sb The MutStr handle.
 * @param fmt The format string.
 * @param ... The arguments to replace in the formatted string.
 *
 * @return MUTSTR_ERROR_NONE on success.
 */
MutStrState mutstr_append_format(MutStr *sb, const char *fmt, ...);

/**
 * Appends a signed integer.
 *
 * @param sb The MutStr handle.
 * @param value The signed integer to append.
 *
 * @return MUTSTR_ERROR_NONE on success.
 */
MutStrState mutstr_append_int(MutStr *sb, int64_t value);

/**
 * Appends an unsigned integer.
 *
 * @param sb The MutStr handle.
 * @param value The unsigned integer to append.
 *
 * @return MUTSTR_ERROR_NONE on success.
 */
MutStrState mutstr_append_uint(MutStr *sb, uint64_t value);

/**
 * Appends a floating point number.
 *
 * @param sb The MutStr handle.
 * @param value The floating point value to append.
 * @param decimals The number of decimals to append after the decimal separator.
 *
 * @return MUTSTR_ERROR_NONE on success.
 */
MutStrState mutstr_append_float(MutStr *sb, double value, int32_t decimals);

/**
 * Converts the string to uppercase.
 *
 * @param sb The MutStr handle.
 */
void mutstr_to_uppercase(MutStr *sb);

/**
 * Converts the string to lowercase.
 *
 * @param sb The StrBuilder handle.
 */
void mutstr_to_lowercase(MutStr *sb);

/**
 * Replaces all occurrences of a character with a different character.
 *
 * @param sb The MutStr handle.
 * @param search The character to search.
 * @param replace The character to replace with.
 *
 * @return The number of replacements made.
 */
int mutstr_replace_char(MutStr *sb, char search, char replace);

/**
 * Repeats the string n times.
 *
 * @param sb The MutStr handle.
 * @param times The number of times that the string should be repeated.
 * This value has to be equal or greater than zero. If the number of times
 * is 0, the string will be truncated to an empty string (length=0).
 *
 * @return MUTSTR_ERROR_NONE on success.
 */
MutStrState mutstr_repeat(MutStr *sb, int times);

/**
 * Strips whitespace from the beginning and end of a string.
 *
 * @param sb The MutStr handle.
 */
void mutstr_trim(MutStr *sb);

/**
 * Splits the string into substrings using a separator.
 *
 * @param sb The MutStr handle.
 * @param pieces The array of MutStr pieces.
 * @param max_pieces The maximum number of pieces to split.
 * @param separator The separator.
 * @param separator_len The length of the separator.
 *
 * @return The number of pieces the string was divided into. This value must be less than or equal to
 * max_pieces.
 */
int mutstr_split(const MutStr *sb, MutStr *pieces, int32_t max_pieces, const char *separator, int32_t separator_len);
