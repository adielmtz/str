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
 * Copies the string of a MutStr object to an uninitialized MutStr object.
 *
 * @param source The source MutStr object to copy from.
 * @param destination The destination MutStr object to copy to. This object must not be initialized,
 * otherwise it will cause a double initialization, resulting in a memory leak.
 */
void mutstr_copy(const MutStr *source, MutStr *destination);

/**
 * Compares two MutStr objects.
 *
 * @param a The first MutStr object.
 * @param b The second MutStr object.
 *
 * @return -1 if the first MutStr object is less than the second MutStr object.
 * 0 if both MutStr objects are equal.
 * 1 if the second MutStr object is greater than the first MutStr object.
 */
int32_t mutstr_compare(const MutStr *a, const MutStr *b);

/**
 * Returns true if both MutStr objects are equal.
 *
 * @param a The first MutStr object.
 * @param b The second MutStr object.
 *
 * @return True if both MutStr objects are the same pointer or have the same string value.
 */
bool mutstr_equals(const MutStr *a, const MutStr *b);

/**
 * Returns the zero-based index of the first occurrence of the substring.
 *
 * @param mutstr The MutStr object.
 * @param substr The substring to search.
 *
 * @return The zero-based index of the first occurrence or -1 if the substring is not found.
 */
int32_t mutstr_indexof(const MutStr *mutstr, const char *substr);

/**
 * Returns true if the MutStr object contains the given substring.
 *
 * @param mutstr The MutStr object.
 * @param substr The substring to search.
 */
bool mutstr_contains(const MutStr *mutstr, const char *substr);

/**
 * Returns true if the MutStr object starts with the given string.
 *
 * @param mutstr The MutStr object.
 * @param prefix The string to compare with.
 */
bool mutstr_starts_with(const MutStr *mutstr, const char *prefix);

/**
 * Returns true if the MutStr object ends with the given string.
 *
 * @param mutstr The MutStr object.
 * @param suffix The string to compare with.
 * @return
 */
bool mutstr_ends_with(const MutStr *mutstr, const char *suffix);

/**
 * Appends the value of another MutStr object.
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
 * @param ...
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
 * Converts the string to uppercase.
 *
 * @param mutstr The MutStr object to convert to uppercase.
 */
void mutstr_to_uppercase(MutStr *mutstr);

/**
 * Converts the string to lowercase.
 *
 * @param mutstr The MutStr object to convert to lowercase.
 */
void mutstr_to_lowercase(MutStr *mutstr);

/**
 * Trims the whitespace off the MutStr object.
 *
 * @param mutstr The MutStr object to trim.
 * @param options Trim options.
 */
void mutstr_trim(MutStr *mutstr, MutStrTrimOptions options);
