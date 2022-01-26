#ifndef STRING_BUILDER_H
#define STRING_BUILDER_H

#include <stdint.h>
#include <stdbool.h>

#define STRING_BUILDER_MIN_SIZE 16

typedef enum StringBuilderError
{
    STRING_BUILDER_ERROR_NONE,
    STRING_BUILDER_ERROR_MEM_ALLOC_FAILURE,
    STRING_BUILDER_ERROR_OUT_OF_RANGE,
} StringBuilderError;

typedef struct StringBuilder
{
    char *str;
    int32_t len;
    int32_t size;
    StringBuilderError error;
} StringBuilder;

/**
 * Sets a custom memory allocator that the StringBuilder API will use.
 *
 * @param malloc_fn
 */
void stringbuilder_set_memory_alloc(void *(*malloc_fn)(size_t));

/**
 * Sets a custom memory reallocator that the StringBuilder API will use.
 *
 * @param realloc_fn
 */
void stringbuilder_set_memory_realloc(void *(*realloc_fn)(void *, size_t));

/**
 * Sets a custom memory deallocator (free) that the StringBuilder API will use.
 *
 * @param free_fn
 */
void stringbuilder_set_memory_free(void (*free_fn)(void *));

/**
 * Initializes a StringBuilder object.
 *
 * @param sb The StringBuilder handle.
 *
 * @return STRING_BUILDER_ERROR_NONE on success.
 */
StringBuilderError stringbuilder_init(StringBuilder *sb);

/**
 * Initializes a StringBuilder object with a custom memory size.
 *
 * @param sb The StringBuilder handle.
 * @param size The amount of memory to allocate.
 *
 * @return STRING_BUILDER_ERROR_NONE on success.
 */
StringBuilderError stringbuilder_init_size(StringBuilder *sb, int32_t size);

/**
 * Releases the memory used by the StringBuilder object.
 *
 * @param sb The StringBuilder handle.
 */
void stringbuilder_finalize(StringBuilder *sb);

/**
 * Copies the string into a different StringBuilder object.
 *
 * @param src The source StringBuilder handle.
 * @param dest The destination StringBuilder handle. This object must not be initialized
 * or else a memory leak may occur.
 *
 * @return STRING_BUILDER_ERROR_NONE on success.
 */
StringBuilderError stringbuilder_copy(StringBuilder *src, StringBuilder *dest);

/**
 * Gets the string pointer.
 *
 * @param sb The StringBuilder handle.
 *
 * @return A pointer to the resulting string.
 */
const char *stringbuilder_get_str(const StringBuilder *sb);

/**
 * Gets the last error code.
 *
 * @param sb The StringBuilder handle.
 *
 * @return StringBuilderError.
 */
StringBuilderError stringbuilder_get_last_error(const StringBuilder *sb);

/**
 * Gets the error string for the given error code.
 *
 * @param code The error code.
 *
 * @return A const char pointer.
 */
const char *stringbuilder_get_error_msg(StringBuilderError code);

/**
 * Gets the length of the string.
 *
 * @param sb The StringBuilder handle.
 */
int32_t stringbuilder_get_length(const StringBuilder *sb);

/**
 * Sets the length of the string.
 *
 * @param sb The StringBuilder handle.
 * @param len The length of the string.
 * <br>If the length is shorter, the string is truncated.
 * <br>If the length is larger, the gap will be padded with NULL characters.
 * The API will reallocate more memory (if necessary) to satisfy the requested length.
 *
 * @return STRING_BUILDER_ERROR_NONE on success.
 */
StringBuilderError stringbuilder_set_length(StringBuilder *sb, int32_t length);

/**
 * Gets the amount of memory allocated for the StringBuilder object.
 *
 * @param sb The StringBuilder handle.
 */
int32_t stringbuilder_get_size(const StringBuilder *sb);

/**
 * Sets the amount of memory allocated for the StringBuilder object.
 *
 * @param sb The StringBuilder handle.
 * @param size The new amount of memory to reallocate.
 * <br>If the size is less than the length, the string will be truncated.
 *
 * @return STRING_BUILDER_ERROR_NONE on success.
 */
StringBuilderError stringbuilder_set_size(StringBuilder *sb, int32_t newSize);

/**
 * Compares 2 strings.
 *
 * @param a The first StringBuilder handle.
 * @param b The second StringBuilder handle.
 *
 * @return Returns a negative integer if the first string is less than the second string.
 * <br>Returns a positive integer if the first string is greater than the second string.
 * <br>Returns zero if both strings are equal.
 */
int stringbuilder_compare(const StringBuilder *a, const StringBuilder *b);

/**
 * Returns true if both strings are equal.
 *
 * @param a The first StringBuilder handle.
 * @param b The second StringBuilder handle.
 */
bool stringbuilder_equals(const StringBuilder *a, const StringBuilder *b);

/**
 * Returns true if the string contains the given substring.
 *
 * @param sb The StringBuilder handle.
 * @param needle The substring to search for.
 * @param needle_len The length of the substring.
 *
 * @return Returns true if the substring is found in the string.
 * An empty needle (needle_len=0) always returns true.
 */
bool stringbuilder_contains(const StringBuilder *sb, const char *needle, int32_t needle_len);

/**
 * Returns true if the string starts with the given substring.
 *
 * @param sb The StringBuilder handle.
 * @param prefix The substring to search for.
 * @param prefix_len The length of the substring.
 *
 * @return Returns true if the string begins with the substring.
 * An empty needle (needle_len=0) always returns true.
 */
bool stringbuilder_starts_with(const StringBuilder *sb, const char *prefix, int32_t prefix_len);

/**
 * Returns true if the string ends with the given substring.
 *
 * @param sb The StringBuilder handle.
 * @param suffix The substring to search for.
 * @param suffix_len The length of the substring.
 *
 * @return Returns true if the string ends with the substring.
 * An empty needle (needle_len=0) always returns true.
 */
bool stringbuilder_ends_with(const StringBuilder *sb, const char *suffix, int32_t suffix_len);

/**
 * Concatenates the string with another StringBuilder object.
 *
 * @param sb The StringBuilder handle.
 * @param other The StringBuilder handle to append.
 *
 * @return STRING_BUILDER_ERROR_NONE on success.
 */
StringBuilderError stringbuilder_concat(StringBuilder *sb, const StringBuilder *other);

/**
 * Appends a character.
 *
 * @param sb The StringBuilder handle.
 * @param c The character to append.
 *
 * @return STRING_BUILDER_ERROR_NONE on success.
 */
StringBuilderError stringbuilder_append_char(StringBuilder *sb, char c);

/**
 * Appends a string.
 *
 * @param sb The StringBuilder handle.
 * @param str The string to append.
 * @param len The length of the string.
 *
 * @return STRING_BUILDER_ERROR_NONE on success.
 */
StringBuilderError stringbuilder_append_string(StringBuilder *sb, const char *string, int32_t len);

/**
 * Appends a formatted string.
 *
 * @param sb The StringBuilder handle.
 * @param fmt The format string.
 * @param ... The arguments to replace in the formatted string.
 *
 * @return STRING_BUILDER_ERROR_NONE on success.
 */
StringBuilderError stringbuilder_append_format(StringBuilder *sb, const char *fmt, ...);

/**
 * Appends a signed integer.
 *
 * @param sb The StringBuilder handle.
 * @param value The signed integer to append.
 *
 * @return STRING_BUILDER_ERROR_NONE on success.
 */
StringBuilderError stringbuilder_append_int(StringBuilder *sb, int64_t value);

/**
 * Appends an unsigned integer.
 *
 * @param sb The StringBuilder handle.
 * @param value The unsigned integer to append.
 *
 * @return STRING_BUILDER_ERROR_NONE on success.
 */
StringBuilderError stringbuilder_append_uint(StringBuilder *sb, uint64_t value);

/**
 * Appends a floating point number.
 *
 * @param sb The StringBuilder handle.
 * @param value The floating point value to append.
 * @param decimals The number of decimals to append after the decimal separator.
 *
 * @return STRING_BUILDER_ERROR_NONE on success.
 */
StringBuilderError stringbuilder_append_float(StringBuilder *sb, double value, int32_t decimals);

/**
 * Converts the string to uppercase.
 *
 * @param sb The StringBuilder handle.
 */
void stringbuilder_to_uppercase(StringBuilder *sb);

/**
 * Converts the string to lowercase.
 *
 * @param sb The StrBuilder handle.
 */
void stringbuilder_to_lowercase(StringBuilder *sb);

/**
 * Replaces all occurrences of a character with a different character.
 *
 * @param sb The StringBuilder handle.
 * @param search The character to search.
 * @param replace The character to replace with.
 *
 * @return The number of replacements made.
 */
int stringbuilder_replace_char(StringBuilder *sb, char search, char replace);

/**
 * Repeats the string n times.
 *
 * @param sb The StringBuilder handle.
 * @param times The number of times that the string should be repeated.
 * This value has to be equal or greater than zero. If the number of times
 * is 0, the string will be truncated to an empty string (len=0).
 *
 * @return STRING_BUILDER_ERROR_NONE on success.
 */
StringBuilderError stringbuilder_repeat(StringBuilder *sb, int times);

/**
 * Strips whitespace from the beginning and end of a string.
 *
 * @param sb The StringBuilder handle.
 */
void stringbuilder_trim(StringBuilder *sb);

#endif // STRING_BUILDER_H
