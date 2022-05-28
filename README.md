# MutStr

A mutable string library for C.

## TODO:

- [x] Append NULL terminator automatically.
- [x] Allow MutStr to be allocated on the stack.
- [x] Allow custom memory allocators (malloc, realloc, free).
- [x] to_uppercase & to_lowercase functions.
- [ ] substr function.
- [ ] replace_string function.
- [x] indexof function.
- [x] indexof function.

## Usage

Copy `mutstr.h` and `mutstr.c` to your project directory and then `#include` it:

```c
#include "mutstr.h"
```

### Allocation

Use the function `mutstr_init` or `mutstr_init_sz` to initialize an MutStr object. After you're
done with your string, you can use `mutstr_finalize` to deallocate any memory and resources used by the API.

```c
MutStr sb;

// Allocate using the default memory size
mutstr_init(&sb);

// Allocate using a custom memory size
mutstr_init_sz(&sb, 1024);

if (mutstr_get_err(&sb) == STRING_BUILDER_ERROR_NONE) {
    // Success!
    
    // Free memory after you're done.
    mutstr_finalize(&sb);
}
```

This snippet illustrates a basic example:

```c
#include "mutstr.h"
#include <stdio.h>

int main()
{
    MutStr sb;
    mutstr_init(&sb);
    if (mutstr_get_last_error(&sb) != STRING_BUILDER_ERROR_NONE) {
        // Get an error message if something failed
        printf("PANIK! %s\n", mutstr_get_error_msg(&sb));
        return EXIT_FAILURE;
    }
    
    // Work with your MutStr object...
    mutstr_append_string(&sb, "Year: ", 6);
    mutstr_append_int(&sb, 2022);
    
    // Use the string
    const char *value = mutstr_get_str(&sb);
    
    // Ready to use, as the API automatically appends
    // a NULL terminator.
    printf("Value: \"%s\"\n", value);
    
    // Finalize
    mutstr_finalize(&sb);
    return EXIT_SUCCESS;
}
```

Using `mutstr_copy_to` allows you to create a copy of your string:

```c
MutStr src;
mutstr_init(&src);
mutstr_append_string(&src, "Copy me!", 8);

MutStr dest;
mutstr_copy_to(&src, &dest);

// ...

mutstr_finalize(&dest);
```

### Get/Set the length of the string

With `mutstr_get_len` you can get the length of the string. With `mutstr_set_len` you can set or truncate
the length of the string. If the value is greater than the original length, then the "extra" space will be filled with
NULL chars. If the value is less than the original length, then the string will be truncated.

```c
MutStr sb;
mutstr_init(&sb);
mutstr_append_string(&sb, "Hello world!", 12);

int32_t length = mutstr_get_len(&sb); // length=12
mutstr_set_len(&sb, 5);               // Truncates to: "Hello"
mutstr_set_len(&sb, 10);              // Expands to:   "Hello00000" (0 is the NULL char)
```

### Get/Set the size of the string

The size refers to the total amount of memory allocated for the actual string. This value is always larger than the
length of the string: `size >= length + 1`.

Reducing the memory to a value lower than `length`, will truncate the string to `size - 1`:

```c
MutStr sb;
mutstr_init_sz(&sb, 64);
mutstr_append_string(&sb, "Watame is best sheep", 20);

int32_t size = mutstr_get_size(&sb); // size=64
mutstr_set_size(&sb, 4);             // Truncates the string (length=size - 1): "Wat"
```

### String comparison

Using `mutstr_compare`:

* Returns a negative integer if the first string is less than the second string.
* Returns a positive integer if the first string is greater than the second string.
* Returns zero if both strings are equal.

```c
MutStr a, b;
mutstr_init(&a);
mutstr_init(&b);

mutstr_append_char(&a, 'A');
mutstr_append_char(&b, 'B');

int cmp = mutstr_compare(&a, &b); // -1 : A < B
```

Using `mutstr_equals`:

* Returns true if both strings are equal.

```c
MutStr a, b;
mutstr_init(&a);
mutstr_init(&b);

mutstr_append_char(&a, 'A');
mutstr_append_char(&b, 'A');

bool equal = mutstr_equals(&a, &b); // True
```

### Concatenation

You can concatenate strings using `mutstr_append_*` functions:

```c
// Concatenate another MutStr object:
mutstr_concat(&sb1, &sb2);

// Concatenate a single character:
mutstr_append_char(&sb, 'C');

// Concatenate a string:
const char *text = "Hello world!";
mutstr_append_string(&sb, text, strlen(text));

// Concatenate an integer:
mutstr_append_int(&sb, 2022);
mutstr_append_int(&sb, -1234);

// Concatenate an unsigned integer:
mutstr_append_uint(&sb, 4096);

// Concatenate a float or double value:
mutstr_append_float(&sb, 3.1416);
mutstr_append_float(&sb, -2.7182);

// Concatenate a formatted string:
mutstr_append_format(&sb, "Formatted %s are the %s!", "strings", "best");
```

This API is binary safe, you can append a string that contains NULL chars:

```c
mutstr_append_char(&sb, '\0'); // OK!
mutstr_append_string(&sb, "Contains\0NULL\0chars!", 20); // Works as long as you know the length
```

### Split
With `mutstr_split` you can divide the string into many substrings using a separator:

```c
#define MAX_PIECES 3

MutStr sb;
mutstr_init(&sb);

mutstr_append_string(&sb, "ABC,,DEF,,GHI", 13);

MutStr pieces[MAX_PIECES];
int count = mutstr_split(&sb, pieces, MAX_PIECES, ",,", 2);

for (int i = 0; i < count; i++) {
    const char *value = mutstr_get_str(&pieces[i]);
    printf("Split: '%s'\n", value);
    mutstr_finalize(&pieces[i]);
}

// == OUTPUT ==
// Split: 'ABC'
// Split: 'DEF'
// Split: 'GHI'

mutstr_finalize(&sb);
```

### Get the resulting string

Use either the `value` property or `mutstr_get_str` function to get the string pointer. This API automatically
appends a NULL terminator to your string, so you can use it right away!

```c
const char *value = mutstr_get_str(&sb); // Or sb.value
printf("String: %s\n", value);
```
