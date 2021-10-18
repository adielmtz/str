# StrBuilder
A string builder library for C. This API is quite memory expensive right now, I will try to optimize it.

## Usage
First, copy `strbuilder.h` and `strbuilder.c` to your project directory and then `#include "strbuilder.h"` it.

### Allocation
To allocate a StrBuilder object, you can use `strbuilder_create` or `strbuilder_create_sz` functions. After usage, you
can use `strbuilder_free` to deallocate the StrBuilder object.

```c
StrBuilder *sb;

// Allocate using the default memory size (uses STRBUILDER_DEFAULT_SIZE macro):
if (strbuilder_create(&sb) == STRBUILDER_ERROR_NONE) {
    // Success!
}

// Allocate using a custom memory size:
if (strbuilder_create_sz(&sb, 1024) == STRBUILDER_ERROR_NONE) {
    // Good to go!
}

// Free memory. It won't complain if you pass a NULL pointer~
strbuilder_free(sb);
```

This snippet illustrates a simple usage:

```c
#include "strbuilder.h"
#include <stdio.h>

int main()
{
    StrBuilder *sb;
    if (strbuilder_create(&sb) == STRBUILDER_ERROR_NONE) {
        // Work with your StrBuilder object...
        strbuilder_append_str(sb, "Hello world!", sizeof("Hello world!")-1);
        // Free it!
        strbuilder_free(sb);
    } else {
        printf("PANIK! Something went wrong!");
    }
    
    return 0;
}
```

### Get/Set the length of the string
With `strbuilder_get_len` you can get the length of the string.
With `strbuilder_set_len` you can set or truncate the length of the string.
If the value is greater than the original length, then the "extra" space will be filled with NULL chars.
If the value is less than the original length, then the string will be truncated.

For instance, a StrBuilder object holding the string `Hello world!`:
```c
size_t length = strbuilder_get_len(sb); // 12
strbuilder_set_len(sb, 5); // Truncates to: "Hello"
strbuilder_set_len(sb, 10); // Expands to: "Hello00000" (0 is the NULL char)
```

### Concatenation
You can concatenate strings using `strbuilder_append_*` functions:

```c
// Concatenate a different StrBuilder object:
strbuilder_append(sb1, sb2);

// Concatenate a character:
strbuilder_append_c(sb, 'C');

// Concatenate a string:
const char *text = "Hello world!";
strbuilder_append_str(sb, text, strlen(text));

// Concatenate an integer
strbuilder_append_i(sb, 1024);

// Concatenate an unsigned integer
strbuilder_append_ui(sb, 1024u);
```

This API is binary safe, so you can pass a string containing NULL chars without any issue:

```c
strbuilder_append_c(sb, '\0'); // OK!
strbuilder_append_str(sb, "Contains\0NULL\0chars!", 20); // Works as long as you know the length
```

### Get the resulting string
Use `strbuilder_to_cstr` to get a C-style string:
```c
char *str = strbuilder_to_cstr(sb);
printf("My string is: %s\n", str);
free(str); // Don't forget to free it after you're done with it!
```
This function allocates a new char* buffer and copies the string into it, appending a NULL-char at the end.

Furthermore, you can use `strbuilder_get_cstr` to get the internal char pointer used by the object, avoiding an
additional memory allocation:
```c
const char *str = strbuilder_get_cstr(sb);
printf("My string is: %s\n", str);
```
Please note that this function does not guarantee that the string is NULL-terminated as it just returns the internal
pointer used by the StrBuilder object; therefore you shouldn't try to free it or modify it yourself as it may cause trouble.
