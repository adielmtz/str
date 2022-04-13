# StringBuilder

A string builder library for C.

## TODO:

- [x] Append NULL terminator automatically.
- [x] Allow StringBuilder to be allocated on the stack.
- [x] Allow custom memory allocators (malloc, realloc, free).
- [x] stringbuilder_to_uppercase & stringbuilder_to_lowercase functions.
- [ ] substr function.
- [ ] replace_string function.
- [x] stringbuilder_index_of function.

## Usage

Copy `stringbuilder.h` and `stringbuilder.c` to your project directory and then `#include` it:

```c
#include "stringbuilder.h"
```

### Allocation

Use the function `stringbuilder_init` or `stringbuilder_init_sz` to initialize an StringBuilder object. After you're
done with your string, you can use `stringbuilder_finalize` to deallocate any memory and resources used by the API.

```c
StringBuilder sb;

// Allocate using the default memory size
stringbuilder_init(&sb);

// Allocate using a custom memory size
stringbuilder_init_sz(&sb, 1024);

if (stringbuilder_get_err(&sb) == STRING_BUILDER_ERROR_NONE) {
    // Success!
    
    // Free memory after you're done.
    stringbuilder_finalize(&sb);
}
```

This snippet illustrates a basic example:

```c
#include "stringbuilder.h"
#include <stdio.h>

int main()
{
    StringBuilder sb;
    stringbuilder_init(&sb);
    if (stringbuilder_get_last_error(&sb) != STRING_BUILDER_ERROR_NONE) {
        // Get an error message if something failed
        printf("PANIK! %s\n", stringbuilder_get_error_msg(&sb));
        return EXIT_FAILURE;
    }
    
    // Work with your StringBuilder object...
    stringbuilder_append_string(&sb, "Year: ", 6);
    stringbuilder_append_int(&sb, 2022);
    
    // Use the string
    const char *value = stringbuilder_get_str(&sb);
    
    // Ready to use, as the API automatically appends
    // a NULL terminator.
    printf("Value: \"%s\"\n", value);
    
    // Finalize
    stringbuilder_finalize(&sb);
    return EXIT_SUCCESS;
}
```

Using `stringbuilder_copy` allows you to create a copy of your string:

```c
StringBuilder src;
stringbuilder_init(&src);
stringbuilder_append_string(&src, "Copy me!", 8);

StringBuilder dest;
stringbuilder_copy(&src, &dest);

// ...

stringbuilder_finalize(&dest);
```

### Get/Set the length of the string

With `stringbuilder_get_len` you can get the length of the string. With `stringbuilder_set_len` you can set or truncate
the length of the string. If the value is greater than the original length, then the "extra" space will be filled with
NULL chars. If the value is less than the original length, then the string will be truncated.

```c
StringBuilder sb;
stringbuilder_init(&sb);
stringbuilder_append_string(&sb, "Hello world!", 12);

int32_t length = stringbuilder_get_len(&sb); // length=12
stringbuilder_set_len(&sb, 5);               // Truncates to: "Hello"
stringbuilder_set_len(&sb, 10);              // Expands to:   "Hello00000" (0 is the NULL char)
```

### Get/Set the size of the string

The size refers to the total amount of memory allocated for the actual string. This value is always larger than the
length of the string: `size >= length + 1`.

Reducing the memory to a value lower than `length`, will truncate the string to `size - 1`:

```c
StringBuilder sb;
stringbuilder_init_sz(&sb, 64);
stringbuilder_append_string(&sb, "Watame is best sheep", 20);

int32_t size = stringbuilder_get_size(&sb); // size=64
stringbuilder_set_size(&sb, 4);             // Truncates the string (length=size - 1): "Wat"
```

### String comparison

Using `stringbuilder_compare`:

* Returns a negative integer if the first string is less than the second string.
* Returns a positive integer if the first string is greater than the second string.
* Returns zero if both strings are equal.

```c
StringBuilder a, b;
stringbuilder_init(&a);
stringbuilder_init(&b);

stringbuilder_append_char(&a, 'A');
stringbuilder_append_char(&b, 'B');

int cmp = stringbuilder_compare(&a, &b); // -1 : A < B
```

Using `stringbuilder_equals`:

* Returns true if both strings are equal.

```c
StringBuilder a, b;
stringbuilder_init(&a);
stringbuilder_init(&b);

stringbuilder_append_char(&a, 'A');
stringbuilder_append_char(&b, 'A');

bool equal = stringbuilder_equals(&a, &b); // True
```

### Concatenation

You can concatenate strings using `stringbuilder_append_*` functions:

```c
// Concatenate another StringBuilder object:
stringbuilder_concat(&sb1, &sb2);

// Concatenate a single character:
stringbuilder_append_char(&sb, 'C');

// Concatenate a string:
const char *text = "Hello world!";
stringbuilder_append_string(&sb, text, strlen(text));

// Concatenate an integer:
stringbuilder_append_int(&sb, 2022);
stringbuilder_append_int(&sb, -1234);

// Concatenate an unsigned integer:
stringbuilder_append_uint(&sb, 4096);

// Concatenate a float or double value:
stringbuilder_append_float(&sb, 3.1416);
stringbuilder_append_float(&sb, -2.7182);

// Concatenate a formatted string:
stringbuilder_append_format(&sb, "Formatted %s are the %s!", "strings", "best");
```

This API is binary safe, you can append a string that contains NULL chars:

```c
stringbuilder_append_char(&sb, '\0'); // OK!
stringbuilder_append_string(&sb, "Contains\0NULL\0chars!", 20); // Works as long as you know the length
```

### Split
With `stringbuilder_split` you can divide the string into many substrings using a separator:

```c
#define MAX_PIECES 3

StringBuilder sb;
stringbuilder_init(&sb);

stringbuilder_append_string(&sb, "ABC,,DEF,,GHI", 13);

StringBuilder pieces[MAX_PIECES];
int count = stringbuilder_split(&sb, pieces, MAX_PIECES, ",,", 2);

for (int i = 0; i < count; i++) {
    const char *value = stringbuilder_get_str(&pieces[i]);
    printf("Split: '%s'\n", value);
    stringbuilder_finalize(&pieces[i]);
}

// == OUTPUT ==
// Split: 'ABC'
// Split: 'DEF'
// Split: 'GHI'

stringbuilder_finalize(&sb);
```

### Get the resulting string

Use either the `value` property or `stringbuilder_get_str` function to get the string pointer. This API automatically
appends a NULL terminator to your string, so you can use it right away!

```c
const char *value = stringbuilder_get_str(&sb); // Or sb.value
printf("String: %s\n", value);
```
