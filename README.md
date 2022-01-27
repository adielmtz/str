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
    const char *str = stringbuilder_get_str(&sb);
    
    // Ready to use, as the API automatically appends
    // a NULL terminator.
    printf("Value: \"%s\"\n", str);
    
    // Finalize
    stringbuilder_finalize(&sb);
    return EXIT_SUCCESS;
}
```

### Get/Set the length of the string

With `stringbuilder_get_len` you can get the length of the string. With `stringbuilder_set_len` you can set or truncate
the length of the string. If the value is greater than the original length, then the "extra" space will be filled with
NULL chars. If the value is less than the original length, then the string will be truncated.

For instance, a StringBuilder object holding the string `Hello world!`:

```c
// "Hello world!"
size_t length = stringbuilder_get_len(&sb); // length=12
stringbuilder_set_len(&sb, 5);              // Truncates to: "Hello"
stringbuilder_set_len(&sb, 10);             // Expands to:   "Hello00000" (0 is the NULL char)
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

### Get the resulting string

Use either the `str` property or `stringbuilder_get_str` function to get the string pointer. This API automatically
appends a NULL terminator to your string, so you can use it right away!

```c
const char *str = stringbuilder_get_str(&sb); // Or sb.str
printf("String: %s\n", str);
```
