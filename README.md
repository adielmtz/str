# StrBuilder
A string builder library for C. This API is quite memory expensive right now, I will try to optimize it.

## Usage
First, copy `strbuilder.h` and `strbuilder.c` to your project directory and then `#include "strbuilder.h"` it.

### Allocation
To allocate a StrBuilder object, you can use `strbuilder_create` or `strbuilder_create_sz` functions. After usage, you
can use `strbuilder_free` to deallocate the StrBuilder object.

```c
StrBuilder *sb;
StrBuilderErr err;

// Allocate using the default memory size:
err = strbuilder_create(&sb);

// Allocate using a custom memory size:
err = strbuilder_create_sz(&sb, 1024);

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
    StrBuilderErr err;    
    err = strbuilder_create(&sb);
    
    if (err == STRBUILDER_ERROR_NONE) {
        printf("Good to go!\n");
    } else {
        printf("PANIK! %s\n", strbuilder_get_error_str(err));
    }
    
    strbuilder_free(sb);
    return 0;
}
```

### Get/Set the length of the string
With `strbuilder_get_len` you can get the length of the string.
With `strbuilder_set_len` you can set or truncate the length of the string.
If the value is greater than the original length, then the "extra" space will be filled with NULL chars.
If the value is less than the original length, then the string will be truncated.

```c
size_t length = strbuilder_get_len(sb); // 12 "Hello world!"
strbuilder_set_len(sb, 10); // Truncates to: "Hello worl"
strbuilder_set_len(sb, 20); // Expands to: "Hello worl0000000000" (0 is the NULL char)
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
