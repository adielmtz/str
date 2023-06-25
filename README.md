# Str

A string library for C.

## Usage

Copy 'str.h' & 'str.c' to your project directory and include it:

```c
#include "str.h"
```

## Initialization

Use the functions `str_init()` or `str_init_size()` to initialize a Str object.

```c
Str str;

str_init(&str);
// or
str_init_size(&str, 32);
```

After you're done working with your string, call `str_finalize()` to clean up resources:

```c
str_finalize(&str);
```

And lastly, with `str_copy()` you can make a copy of an existing Str object:

```c
Str first;

str_init(&first);
str_append_str(&first, "Copy me!", -1);

Str second; // Must not be initialized otherwise a memory leak may occur!
str_copy(&first, &second);

// Finalize both objects!
str_finalize(&first);
str_finalize(&second);
```

## String Properties

### Get/Set the length the string

Str exposes its `length` and `size` properties for read-only purposes. However, altering their values may result in
unpredictable behaviour of the API which can cause a segfault.

The length of the string counts how many bytes represent the actual string contents in memory, and it does not count
the NULL terminator. To change the length of the string you must use the `str_set_length()` function:

* If the value is greater than the original length, the gap/extra space will be filled with `\0`.
* If the value is less than the original length, the string will be truncated.

```c
str_append_str(&str, "Hello world!", -1);

int64_t old_length = str.length; // 12
str_set_length(&str, 5);         // Truncates to: "Hello"
str_set_length(&str, 10);        // Expands to:   "Hello\0\0\0\0\0"
```

### Get/Set the size of the string

The size of the Str object refers to the amount of allocated memory. This value is always larger than the
length of the string: `size >= length + 1`.

The API keeps track of the allocated memory using the `size` field. To change it, you must use the `str_set_size()`
function:

* If the size is less than or equal to the length of the string, the length will be truncated to `size - 1`.

```c
str_append_str(&str, "Watame is best sheep", -1);
int64_t old_size = str.size; // size=64
str_set_size(&str, 4);       // Truncates to: "Wat" (length = size - 1)
```

## String comparison

`str_compare`:

* Returns a negative integer if the first string is less than the second string.
* Returns a positive integer if the first string is greater than the second string.
* Returns zero if both strings are equal.

```c
Str a, b;
str_init(&a);
str_init(&b);

str_append_char(&a, 'A');
str_append_char(&b, 'B');

int64_t result = str_compare(&a, &b); // result = -1 : A < B

str_finalize(&a);
str_finalize(&b);
```

`str_equals`:

* Returns true if both strings are equal.

```c
Str a, b;
str_init(&a);
str_init(&b);

str_append_str(&a, "XYZ", 3);
str_append_str(&b, "XYZ", 3);

bool result = str_equals(&a, &b); // result: true

str_finalize(&a);
str_finalize(&b);
```

## Concatenation

You can concatenate strings (and other types of values) using the `str_append_*()` functions:

```c
// Concatenate another Str object:
str_concat(&str, &other);

// Concatenate a character:
str_append_char(&str, 'C');

// Concatenate a const string literal
str_append_str(&str, "Hello world!", -1);

// Concatenate a signed integer:
str_append_int(&str, 2022);
str_append_int(&str, -128);

// Concatenate an unsigned integer:
str_append_uint(&str, 4096);

// Concatenate a float value:
str_append_float(&str, M_PI, 4);
str_append_float(&str, 100.30, 2);

// Concatenate a formatted string:
str_append_format(&str, "Formatted %s are the %s!", "strings", "best");
```

This API is binary safe, you can append a string that contains NULL chars:

```c
str_append_char(&sb, '\0'); // OK!
str_append_str(&sb, "Contains\0NULL\0chars!", 20); // Works as long as you know the length
```

## Trim

Use `str_trim()` function to trim whitespace off the string:

```c
str str;
str_init(&str);

str_append_str(&str, "    Padded String      ");

// Trim left side
str_trim(&str, STR_TRIM_LEFT); // "Padded String      "

// Trim right side
str_trim(&str, STR_TRIM_RIGHT); // "    Padded String"

// Trim both sides
str_trim(&str, STR_TRIM_BOTH); // "Padded String"

str_finalize(&str);
```
