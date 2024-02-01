encode
======

Simple C library for Base16, Base32 and Base64 encoding/decoding.  
Based on [RFC 4638](https://www.rfc-editor.org/rfc/pdfrfc/rfc4648.txt.pdf).

Building:

```
    TODO
```

Usage:

```
#include <encode.h>

void to_base16(const unsigned char *input, size_t input_length, char **output);
void to_base32(const unsigned char *input, size_t input_length, char **output);
void to_base64(const unsigned char *input, size_t input_length, char **output);

size_t from_base16(const char *input, unsigned char **output);
size_t from_base32(const char *input, unsigned char **output);
size_t from_base64(const char *input, unsigned char **output);
```

Example:

```
#include <stdio.h>
#include <encode.h>

char *data = "foobar";
char *encoded_data = NULL;
unsigned char *decoded_data = NULL;

// Encoding
to_base64((unsigned char *)data, strlen(data), &encoded_data);
if (encoded_data != NULL)
{
    /* Outputs Zm9vYmFy to stdout */
    printf("ENCODED: %s\n", encoded_data);
}

// Decoding
size_t length = from_base64(encoded_data, &decoded_data);
if (decoded_data != NULL)
{
    /* Outputs foobar to stdout */
    printf("DECODED: %.*s\n", length, decoded_data);
}

free(encoded_data);
free(decoded_data);
```

