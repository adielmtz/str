# StrBuilder
A string builder library for C.

### Usage
First, copy `strbuilder.h` and `strbuilder.c` to your project directory
and then `#include "strbuilder.h"` it.

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

    // Free memory. It won't complain if sb is NULL!
    strbuilder_free(sb);
    return 0;
}
```

