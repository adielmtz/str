# StrBuilder
A string builder library for C. This API is quite memory expensive right now, I will try to optimize it.

### Usage
First, copy `strbuilder.h` and `strbuilder.c` to your project directory
and then `#include "strbuilder.h"` it.

#### Allocation
This snippet illustrates a basic usage:
```c
#include "strbuilder.h"
#include <stdio.h>

int main()
{
    StrBuilder *sb;
    StrBuilderErr err;

    // Allocate using the default memory size:
    err = strbuilder_create(&sb);

    // Allocate using a custom memory size:
    err = strbuilder_create_sz(&sb, 1024);

    // Check if something went wrong:
    if (err != STRBUILDER_SUCCESS) {
        printf("PANIK! %s\n", strbuilder_get_error_str(err));
    } else {
        printf("Good to go!\n");
    }

    // Free memory. It won't complain if you pass a NULL pointer~
    strbuilder_free(sb);
    return 0;
}
```

#### Concatenation
After allocating a StrBuilder object, you can concatenate strings using `strbuilder_append_*` functions:
```c
StrBuilder err;

// To concatenate a different StrBuilder object:
err = strbuilder_append(sb1, sb2);

// To concatenate a char:
err = strbuilder_append_c(sb, 'C');

// Concatenate a string literal
const char *text = "Hello world!";
err = strbuilder_append_str(sb, text, strlen(text));

// Concatenate an integer
err = strbuilder_append_i(sb, 1024);

// Concatenate an unsigned integer
err = strbuilder_append_ui(sb, 1024u);
```

The API is binary safe, so you can pass a NULL char without any issue:
```c
strbuilder_append_c(sb, '\0'); // OK!
strbuilder_append_str(sb, "Contains\0NULL\0chars", 19); // Works as long as you know the length
```

#### Get the resulting string
Use `strbuilder_to_cstr` to get a C-style string:
```c
char *str = strbuilder_to_cstr(sb);
printf("My string is: %s\n", str);
free(str); // don't forget to free it after you're done with it!
```
