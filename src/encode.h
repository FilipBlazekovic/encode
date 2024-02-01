#ifndef ENCODE_H
#define ENCODE_H

#include <ctype.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

void to_base16(const unsigned char *input, size_t input_length, char **output);
void to_base32(const unsigned char *input, size_t input_length, char **output);
void to_base64(const unsigned char *input, size_t input_length, char **output);

size_t from_base16(const char *input, unsigned char **output);
size_t from_base32(const char *input, unsigned char **output);
size_t from_base64(const char *input, unsigned char **output);

#endif //ENCODE_H
