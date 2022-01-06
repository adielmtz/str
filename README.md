# StrBuilder

A string builder library for C.

## TODO:

- [x] Allow StrBuilder to be allocated on the stack.
- [x] Allow custom memory allocators (malloc, realloc, free).
- [x] to_uppercase & to_lowercase functions.
- [ ] substr function.
- [ ] replace_str function.

## Usage

Copy `strbuilder.h` and `strbuilder.c` to your project directory and then `#include` it:

```c
#include "strbuilder.h"
```

### Allocation

Use the function `strbuilder_init` or `strbuilder_init_sz` to initialize an StrBuilder object. After you're done with
your string, you can use `strbuilder_finalize` to deallocate any memory and resources used by the API.

```c
StrBuilder sb;

// Allocate using the default memory size
strbuilder_init(&sb);

// Allocate using a custom memory size
strbuilder_init_sz(&sb, 1024);

if (strbuilder_get_err(&sb) == STRBUILDER_ERROR_NONE) {
    // Success!
    
    // Free memory after you're done.
    strbuilder_finalize(&sb);
}
```

This snippet illustrates a basic example:

```c
#include "strbuilder.h"
#include <stdio.h>

int main()
{
    StrBuilder sb;
    strbuilder_init(&sb);
    if (strbuilder_get_err(&sb) != STRBUILDER_ERROR_NONE) {
        // Get an error message if something failed
        printf("PANIK! %s\n", strbuilder_get_error_msg(&sb));
        return EXIT_FAILURE;
    }
    
    // Work with your StrBuilder object...
    strbuilder_append_str(&sb, "Year: ", 6);
    strbuilder_append_i(&sb, 2022);
    
    // Add NULL terminator
    strbuilder_append_c(&sb, '\0');
    
    // Use the string
    const char *str = strbuilder_get_str(&sb);
    printf("Value: \"%s\"\n", str);
    
    // Finalize
    strbuilder_finalize(&sb);
    return EXIT_SUCCESS;
}
```

### Get/Set the length of the string

With `strbuilder_get_len` you can get the length of the string. With `strbuilder_set_len` you can set or truncate the
length of the string. If the value is greater than the original length, then the "extra" space will be filled with NULL
chars. If the value is less than the original length, then the string will be truncated.

For instance, a StrBuilder object holding the string `Hello world!`:

```c
// "Hello world!"
size_t length = strbuilder_get_len(&sb); // length=12
strbuilder_set_len(&sb, 5);              // Truncates to: "Hello"
strbuilder_set_len(&sb, 10);             // Expands to:   "Hello00000" (0 is the NULL char)
```

### Concatenation

You can concatenate strings using `strbuilder_append_*` functions:

```c
// Concatenate a different StrBuilder object:
strbuilder_append(&sb1, &sb2);

// Concatenate a character:
strbuilder_append_c(&sb, 'C');

// Concatenate a string:
const char *text = "Hello world!";
strbuilder_append_str(&sb, text, strlen(text));

// Concatenate an integer
strbuilder_append_i(&sb, 2022);
strbuilder_append_i(&sb, -1234);

// Concatenate an unsigned integer
strbuilder_append_ui(&sb, 4096);

// Concatenate a float or double value
strbuilder_append_d(&sb, 3.1416);
strbuilder_append_d(&sb, -2.7182);
```

This API is binary safe, so you can pass a string containing NULL chars without any issue:

```c
strbuilder_append_c(&sb, '\0'); // OK!
strbuilder_append_str(&sb, "Contains\0NULL\0chars!", 20); // Works as long as you know the length
```

### Get the resulting string

Use either the `str` property or `strbuilder_get_str` function to get the string pointer. This API uses the length
property to keep track of the string in memory. Before you use it, you should append a NULL terminator.

```c
strbuilder_append_c(&sb, '\0');            // Appending a NULL terminator
const char *str = strbuilder_get_str(&sb); // Or sb.str
printf("String: %s\n", str);
```
