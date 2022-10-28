# MutStr

A mutable string library for C.

## Usage

Copy `mutstr.h` & `mutstr.c` to your project directory and include it:

```c
#include "mutstr.h"
```

## Initialization

Use the functions `mutstr_init()` or `mutstr_allocate()` to initialize a MutStr object.
After you're done working with your string, use `mutstr_finalize()` to deallocate the memory used by the MutStr object:

```c
#include "mutstr.h"
#include <stdio.h>

int main()
{
    MutStr mutstr;
    mutstr_init(&mutstr);

    mutstr_append_literal(&mutstr, "Hello World!");
    printf("Value: %s\n", mutstr.value);
    
    mutstr_finalize(&mutstr);
    return 0;
}
```

Using `mutstr_copy()` allows you to make a copy of the MutStr object:

```c
MutStr original;
mutstr_init(&original);
mutstr_append_literal(&mutstr, "Copy me!");

MutStr copy; // copy must not be initialized or else a memory leak will occur!
mutstr_copy(&original, &copy);

mutstr_finalize(&original);
mutstr_finalize(&copy);
```

## String Properties

### Get/Set the length the string

MutStr exposes its `length` and `size` properties for read, however, modifying their values may cause a bad behaviour of
the API which can result in a segfault.

The length of the string counts how many bytes represent the actual string contents in memory and it does not count
the NULL terminator. To change the length of the string you must use the `mutstr_set_length()` function:

* If the value is greater than the original length, the gap/extra space will be filled with `\0`.
* If the value is less than the original length, the string will be truncated.

```c
mutstr_append_literal(&mutstr, "Hello world!");
int32_t old_length = mutstr.length; // 12
mutstr_set_length(&mutstr, 5);      // Truncates to: "Hello"
mutstr_set_length(&mutstr, 10);     // Expands to:   "Hello\0\0\0\0\0"
```

### Get/Set the size of the string

The size of the MutStr object refers to the amount of allocated memory. This value is always larger than the
length of the string: `size >= length + 1`.

The API keeps track of the allocated memory using the `size` field. To change it, you must use the `mutstr_set_size()`
function:

* If the size is less than or equal to the length of the string, the length will be truncated to `size - 1`.

```c
mutstr_append_literal(&mutstr, "Watame is best sheep");
int32_t old_size = mutstr.size; // size=64
mutstr_set_size(&mutstr, 4);    // Truncates to: "Wat" (length = size - 1)
```

## String comparison

`mutstr_compare`:

* Returns a negative integer if the first string is less than the second string.
* Returns a positive integer if the first string is greater than the second string.
* Returns zero if both strings are equal.

```c
MutStr a, b;
mutstr_init(&a);
mutstr_init(&b);

mutstr_append_char(&a, 'A');
mutstr_append_char(&b, 'B');

int32_t result = mutstr_compare(&a, &b); // result = -1 : A < B

mutstr_finalize(&a);
mutstr_finalize(&b);
```

`mutstr_equals`:

* Returns true if both strings are equal.

```c
MutStr a, b;
mutstr_init(&a);
mutstr_init(&b);

mutstr_append_literal(&a, "XYZ");
mutstr_append_literal(&b, "XYZ");

bool result = mutstr_equals(&a, &b); // result: true

mutstr_finalize(&a);
mutstr_finalize(&b);
```

## Concatenation

You can concatenate strings (& other values) using the `mutstr_append_*()` functions:

```c
// Concatenate another MutStr object:
mutstr_append_mutstr(&mutstr, &other);

// Concatenate a character:
mutstr_append_char(&mutstr, 'C');

// Concatenate a const string literal
mutstr_append_literal(&mutstr, "Hello world!");

// Concatenate a string (user input, for example):
mutstr_append_string(&mutstr, "Hello", 5);

// Concatenate a signed integer:
mutstr_append_int(&mutstr, 2022);
mutstr_append_int(&mutstr, -128);

// Concatenate an unsigned integer:
mutstr_append_uint(&mutstr, 4096);

// Concatenate a float value:
mutstr_append_float(&mutstr, M_PI, 4);
mutstr_append_float(&mutstr, 100.30, 2);

// Concatenate a formatted string:
mutstr_append_format(&mutstr, "Formatted %s are the %s!", "strings", "best");
```

This API is binary safe, you can append a string that contains NULL chars:

```c
mutstr_append_char(&sb, '\0'); // OK!
mutstr_append_string(&sb, "Contains\0NULL\0chars!", 20); // Works as long as you know the length
```

## Trim

Use `mutstr_trim()` function to trim whitespace off the string:

```c
MutStr mutstr;
mutstr_init(&mutstr);

mutstr_append_literal(&mutstr, "    Padded String      ");

// Trim left side
mutstr_trim(&mutstr, MUTSTR_TRIM_LEFT); // "Padded String      "

// Trim right side
mutstr_trim(&mutstr, MUTSTR_TRIM_RIGHT); // "    Padded String"

// Trim both sides
mutstr_trim(&mutstr, MUTSTR_TRIM_BOTH); // "Padded String"

mutstr_finalize(&mutstr);
```
