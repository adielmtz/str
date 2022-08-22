#pragma once

#include <stdbool.h>
#include <stdint.h>

#define MUTSTR_DEFAULT_INITIAL_SIZE 16

typedef enum MutStrState
{
    MUTSTR_UNINITIALIZED = 0,
    MUTSTR_OK = 1,
    MUTSTR_MEMORY_ALLOC_FAIL = 2,
    MUTSTR_INDEX_OUT_OF_RANGE = 3,
    MUTSTR_UNKNOWN_LENGTH = 4,
} MutStrState;

typedef enum MutStrTrimOptions
{
    MUTSTR_TRIM_LEFT = 1,
    MUTSTR_TRIM_RIGHT = 2,
    MUTSTR_TRIM_BOTH = MUTSTR_TRIM_LEFT | MUTSTR_TRIM_RIGHT,
} MutStrTrimOptions;

typedef struct MutStr
{
    char *value;       // The string pointer. Must be NULL terminated
    int32_t length;    // The length of the string (counts bytes, not actual chars).
    int32_t size;      // The amount of allocated memory for the string (size >= length + 1).
    MutStrState state; // The state of the MutStr object after the last operation/function call.
} MutStr;

/**
 * Initializes the MutStr object using the default memory size.
 *
 * @param mutstr The MutStr object to initialize.
 */
void mutstr_init(MutStr *mutstr);

/**
 * Initializes the MutStr object with a user defined memory size.
 *
 * @param mutstr The MutStr object to initialize.
 * @param initial_size The size of memory to allocate.
 */
void mutstr_init_size(MutStr *mutstr, int32_t initial_size);

/**
 * Deallocates the memory allocated for the MutStr object.
 *
 * @param mutstr
 */
void mutstr_finalize(MutStr *mutstr);

/**
 * Sets the length of the string.
 * If the new length is less than the original length, then the string will be truncated.
 * If the new length is greater than the original length, the gap will be filled with \0 chars.
 *
 * @param mutstr The MutStr object to modify.
 * @param length The new length for the string.
 */
void mutstr_set_length(MutStr *mutstr, int32_t length);

/**
 * Reallocates the memory used by the MutStr object to the given size.
 *
 * @param mutstr The MutStr object to reallocate.
 * @param size The new memory size to allocate.
 */
void mutstr_set_size(MutStr *mutstr, int32_t size);

/**
 * Ensures that the memory size is large enough for the required size.
 * If the memory is smaller, then it will be reallocated to double its size.
 *
 * @param mutstr The MutStr object.
 * @param required_size The required memory size to test.
 */
void mutstr_ensure_size(MutStr *mutstr, int32_t required_size);

/**
 * Copies the value of a MutStr object to an uninitialized MutStr object.
 *
 * @param source The source MutStr object to copy from.
 * @param destination The destination MutStr object to copy to. This object must not be initialized,
 * otherwise it will cause a double initialization, resulting in a memory leak.
 */
void mutstr_copy(const MutStr *source, MutStr *destination);

/**
 * Compares the value of two MutStr objects.
 *
 * @param a The first MutStr object.
 * @param b The second MutStr object.
 *
 * @return -1 if the value of the first MutStr object is less than the value of the second MutStr object.
 * 0 if the value of both MutStr objects are equal.
 * 1 if the value of the second MutStr object is greater than the value of the first MutStr object.
 */
int32_t mutstr_compare(const MutStr *a, const MutStr *b);

/**
 * Compares the value of the MutStr object with the given string.
 *
 * @param mutstr The MutStr object.
 * @param string The string to compare with.
 * @param length The length of the string.
 *
 * @return -1 if the value of the MutStr object is less than the value of the string.
 * 0 if the value of the MutStr object equals the value of the string.
 * 1 if the value of the MutStr object is greater than the value of the string.
 */
int32_t mutstr_compare_string(const MutStr *mutstr, const char *string, int32_t length);

/**
 * Compares the value of the MutStr object with a const string literal.
 *
 * @param mutstr The MutStr object.
 * @param string The string to compare with (must be NULL terminated).
 *
 * @return -1 if the value of the MutStr object is less than the value of the string.
 * 0 if the value of the MutStr object equals the value of the string.
 * 1 if the value of the MutStr object is greater than the value of the string.
 */
int32_t mutstr_compare_literal(const MutStr *mutstr, const char *string);

/**
 * Returns true if the value of both MutStr objects are equal.
 *
 * @param a The first MutStr object.
 * @param b The second MutStr object.
 */
bool mutstr_equals(const MutStr *a, const MutStr *b);

/**
 * Returns true if the value of the MutStr object is equal to the given string.
 *
 * @param mutstr The MutStr object.
 * @param string The string to compare with.
 * @param length The length of the string.
 */
bool mutstr_equals_string(const MutStr *mutstr, const char *string, int32_t length);

/**
 * Returns true if the value of the MutStr object is equal to the const string literal.
 *
 * @param mutstr The MutStr object.
 * @param string The string to compare with (must be NULL terminated).
 */
bool mutstr_equals_literal(const MutStr *mutstr, const char *string);

/**
 * Returns the zero-based index of the first occurrence of the needle.
 *
 * @param mutstr The MutStr object.
 * @param needle The needle to search.
 * @param needle_len The length of the needle.
 *
 * @return The zero-based index of the first occurrence or -1 if the needle is not present in the MutStr object.
 */
int32_t mutstr_indexof_string(const MutStr *mutstr, const char *needle, int32_t needle_len);

/**
 * Returns the zero-based index of the first occurrence of the needle.
 *
 * @param mutstr The MutStr object.
 * @param needle The needle to search (must be NULL terminated).
 *
 * @return The zero-based index of the first occurrence or -1 if the needle is not present in the MutStr object.
 */
int32_t mutstr_indexof_literal(const MutStr *mutstr, const char *needle);

/**
 * Returns true if the value of the MutStr object contains the given needle.
 *
 * @param mutstr The MutStr object.
 * @param needle The needle to search.
 * @param needle_len The length of the needle.
 */
bool mutstr_contains_string(const MutStr *mutstr, const char *needle, int32_t needle_len);

/**
 * Returns true if the value of the MutStr object contains the given needle.
 *
 * @param mutstr The MutStr object.
 * @param needle The needle to search (must be NULL terminated).
 */
bool mutstr_contains_literal(const MutStr *mutstr, const char *needle);

/**
 * Returns true if the value of the MutStr object starts with the given string.
 *
 * @param mutstr The MutStr object.
 * @param prefix The string to compare with.
 * @param prefix_len The length of the string.
 */
bool mutstr_starts_with_string(const MutStr *mutstr, const char *prefix, int32_t prefix_len);

/**
 * Returns true if the value of the MutStr object starts with the given const string literal.
 *
 * @param mutstr The MutStr object.
 * @param prefix The string to compare with (must be NULL terminated).
 */
bool mutstr_starts_with_literal(const MutStr *mutstr, const char *prefix);

/**
 * Returns true if the value of the MutStr object ends with the given string.
 *
 * @param mutstr The MutStr object.
 * @param suffix The string to compare with.
 * @param suffix_len The length of the string.
 */
bool mutstr_ends_with_string(const MutStr *mutstr, const char *suffix, int32_t suffix_len);

/**
 * Returns true if the value of the MutStr object ends with the given const string literal.
 *
 * @param mutstr The MutStr object.
 * @param suffix The string to compare with (must be NULL terminated).
 */
bool mutstr_ends_with_literal(const MutStr *mutstr, const char *suffix);

/**
 * Appends the value of other MutStr object.
 *
 * @param mutstr The MutStr object to append to.
 * @param other The MutStr object to copy from.
 */
void mutstr_append_mutstr(MutStr *mutstr, const MutStr *other);

/**
 * Appends a character.
 *
 * @param mutstr The MutStr object to append to.
 * @param c The character to append.
 */
void mutstr_append_char(MutStr *mutstr, char c);

/**
 * Appends a string.
 *
 * @param mutstr The MutStr object to append to.
 * @param str A pointer to the string to append.
 * @param length The length of the string to append.
 */
void mutstr_append_string(MutStr *mutstr, const char *str, int32_t length);

/**
 * Appends a string literal.
 *
 * @param mutstr The MutStr object to append to.
 * @param str The string literal to append (must be NULL terminated).
 */
void mutstr_append_literal(MutStr *mutstr, const char *str);

/**
 * Appends a formatted string.
 *
 * @param mutstr The MutStr object to append to.
 * @param fmt The format string literal (must be NULL terminated).
 * @param ... The format arguments.
 */
void mutstr_append_format(MutStr *mutstr, const char *fmt, ...);

/**
 * Appends a signed integer.
 *
 * @param mutstr The MutStr object to append to.
 * @param value The value to append.
 */
void mutstr_append_int(MutStr *mutstr, int64_t value);

/**
 * Appends an unsigned integer.
 *
 * @param mutstr The MutStr object to append to.
 * @param value The value to append.
 */
void mutstr_append_uint(MutStr *mutstr, uint64_t value);

/**
 * Appends a float or double value.
 *
 * @param mutstr The MutStr object to append to.
 * @param value The value to append.
 * @param precision The number of digits after the decimal separator.
 */
void mutstr_append_float(MutStr *mutstr, double value, int32_t precision);

/**
 * Transforms the value of the MutStr object to uppercase.
 *
 * @param mutstr The MutStr object to convert to uppercase.
 */
void mutstr_to_uppercase(MutStr *mutstr);

/**
 * Transforms the value of the MutStr object to lowercase.
 *
 * @param mutstr The MutStr object to convert to lowercase.
 */
void mutstr_to_lowercase(MutStr *mutstr);

/**
 * Strips whitespace from the beginning and the end of the value of the MutStr object.
 *
 * @param mutstr The MutStr object to trim.
 * @param options Trim options.
 */
void mutstr_trim(MutStr *mutstr, MutStrTrimOptions options);

/**
 * Returns a part of the string into an uninitialized MutStr object.
 *
 * @param source The source MutStr object.
 * @param index The index in source where the substring starts.
 * @param length The length of the substring.
 * @param result The MutStr object where the substring will be stored. This object must not be initialized,
 * otherwise it will cause a double initialization, resulting in a memory leak.
 */
void mutstr_substr(const MutStr *source, int32_t index, int32_t length, MutStr *result);
