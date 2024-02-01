#include "encode.h"

const uint32_t INVALID_CHARACTER = 255;
const char PADDING_CHARACTER = '=';

const char BASE16_ALPHABET[17] = "0123456789ABCDEF";
const char BASE32_ALPHABET[33] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";
const char BASE64_ALPHABET[65] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static uint8_t decode_base16(char character)
{
    for (int i = 0; i < 16; i++)
    {
        if (toupper(character) == BASE16_ALPHABET[i])
            return i;
    }
    return INVALID_CHARACTER;
}

static uint8_t decode_base32(char character)
{
    for (int i = 0; i < 32; i++)
    {
        if (toupper(character) == BASE32_ALPHABET[i])
            return i;
    }
    return INVALID_CHARACTER;
}

static uint8_t decode_base64(char character)
{
    for (int i = 0; i < 64; i++)
    {
        if (character == BASE64_ALPHABET[i])
            return i;
    }
    return INVALID_CHARACTER;
}

static void encode_base32_group(const unsigned char *input, char *output, const int num_padding_chars)
{
    int i = 0;
    uint8_t values[8];

    switch (num_padding_chars)
    {
        case 6:
            values[0] = input[i] >> 3;
            values[1] = ((input[i] & 0b00000111) << 2) | 0;
            break;
        case 4:
            values[0] = input[i] >> 3;
            values[1] = ((input[i] & 0b00000111) << 2) | (input[i + 1] >> 6);
            values[2] = (input[i + 1] & 0b00111110) >> 1;
            values[3] = ((input[i + 1] & 0b00000001) << 4) | 0;
            break;
        case 3:
            values[0] = input[i] >> 3;
            values[1] = ((input[i] & 0b00000111) << 2) | (input[i + 1] >> 6);
            values[2] = (input[i + 1] & 0b00111110) >> 1;
            values[3] = ((input[i + 1] & 0b00000001) << 4) | (input[i + 2] >> 4);
            values[4] = ((input[i + 2] & 0b00001111) << 1) | 0;
            break;
        case 1:
            values[0] = input[i] >> 3;
            values[1] = ((input[i] & 0b00000111) << 2) | (input[i + 1] >> 6);
            values[2] = (input[i + 1] & 0b00111110) >> 1;
            values[3] = ((input[i + 1] & 0b00000001) << 4) | (input[i + 2] >> 4);
            values[4] = ((input[i + 2] & 0b00001111) << 1) | (input[i + 3] >> 7);
            values[5] = (input[i + 3] & 0b01111100) >> 2;
            values[6] = ((input[i + 3] & 0b00000011) << 3) | 0;
            break;
        default:
            values[0] = input[i] >> 3;
            values[1] = ((input[i] & 0b00000111) << 2) | (input[i + 1] >> 6);
            values[2] = (input[i + 1] & 0b00111110) >> 1;
            values[3] = ((input[i + 1] & 0b00000001) << 4) | (input[i + 2] >> 4);
            values[4] = ((input[i + 2] & 0b00001111) << 1) | (input[i + 3] >> 7);
            values[5] = (input[i + 3] & 0b01111100) >> 2;
            values[6] = ((input[i + 3] & 0b00000011) << 3) | (input[i + 4] >> 5);
            values[7] = input[i + 4] & 0b00011111;
    }

    for (int j = 0; j < (8 - num_padding_chars); j++)
        *output++ = BASE32_ALPHABET[values[j]];

    for (int j = 0; j < num_padding_chars; j++)
        *output++ = PADDING_CHARACTER;
}

static size_t decode_base32_group(const char *input, unsigned char *output, const int num_padding_chars)
{
    const uint8_t MASK = 0b00011111;

    uint8_t values[8];
    for (int i = 0; i < (8 - num_padding_chars); i++)
    {
        values[i] = decode_base32(*input++);
        if (values[i] == INVALID_CHARACTER)
            return 0;
    }

    switch (num_padding_chars)
    {
        case 6:
            *output = (values[0] << 3) | ((values[1] & MASK) >> 2);
            return 1;
        case 4:
            *output++ = (values[0] << 3) | ((values[1] & MASK) >> 2);
            *output = ((values[1] << 6) | ((values[2] & MASK) << 1)) | ((values[3] & MASK) >> 4);
            return 2;
        case 3:
            *output++ = (values[0] << 3) | ((values[1] & MASK) >> 2);
            *output++ = ((values[1] << 6) | ((values[2] & MASK) << 1)) | ((values[3] & MASK) >> 4);
            *output = (values[3] << 4) | ((values[4] & MASK) >> 1);
            return 3;
        case 1:
            *output++ = (values[0] << 3) | ((values[1] & MASK) >> 2);
            *output++ = ((values[1] << 6) | ((values[2] & MASK) << 1)) | ((values[3] & MASK) >> 4);
            *output++ = (values[3] << 4) | ((values[4] & MASK) >> 1);
            *output = ((values[4] << 7) | ((values[5] & MASK) << 2)) | ((values[6] & MASK) >> 3);
            return 4;
        default:
            *output++ = (values[0] << 3) | ((values[1] & MASK) >> 2);
            *output++ = ((values[1] << 6) | ((values[2] & MASK) << 1)) | ((values[3] & MASK) >> 4);
            *output++ = (values[3] << 4) | ((values[4] & MASK) >> 1);
            *output++ = ((values[4] << 7) | ((values[5] & MASK) << 2)) | ((values[6] & MASK) >> 3);
            *output = (values[6] << 5) | (values[7] & MASK);
            return 5;
    }
}

static void encode_base64_group(const unsigned char *input, char *output, const int num_padding_chars)
{
    int i = 0;
    uint8_t values[4];

    switch (num_padding_chars)
    {
        case 2:
            values[0] = input[i] >> 2;
            values[1] = ((input[i] & 0b00000011) << 4) | 0;
            break;
        case 1:
            values[0] = input[i] >> 2;
            values[1] = ((input[i] & 0b00000011) << 4) | (input[i + 1] >> 4);
            values[2] = ((input[i + 1] & 0b00001111) << 2) | 0;
            break;
        default:
            values[0] = input[i] >> 2;
            values[1] = ((input[i] & 0b00000011) << 4) | (input[i + 1] >> 4);
            values[2] = ((input[i + 1] & 0b00001111) << 2) | (input[i + 2] >> 6);
            values[3] = input[i + 2] & 0b00111111;
    }

    for (int j = 0; j < (4 - num_padding_chars); j++)
        *output++ = BASE64_ALPHABET[values[j]];

    for (int j = 0; j < num_padding_chars; j++)
        *output++ = PADDING_CHARACTER;
}

static size_t decode_base64_group(const char *input, unsigned char *output, const int num_padding_chars)
{
    const uint8_t SECOND_VALUE_MASK = 0b00110000;
    const uint8_t THIRD_VALUE_MASK = 0b00111100;

    uint8_t values[4];
    for (int i = 0; i < (4 - num_padding_chars); i++)
    {
        values[i] = decode_base64(*input++);
        if (values[i] == INVALID_CHARACTER)
            return 0;
    }

    switch (num_padding_chars)
    {
        case 2:
            *output = (values[0] << 2) | ((values[1] & SECOND_VALUE_MASK) >> 4);
            return 1;
        case 1:
            *output++ = (values[0] << 2) | ((values[1] & SECOND_VALUE_MASK) >> 4);
            *output = (values[1] << 4) | ((values[2] & THIRD_VALUE_MASK) >> 2);
            return 2;
        default:
            *output++ = (values[0] << 2) | ((values[1] & SECOND_VALUE_MASK) >> 4);
            *output++ = (values[1] << 4) | ((values[2] & THIRD_VALUE_MASK) >> 2);
            *output = (values[2] << 6) | values[3];
            return 3;
    }
}

void to_base16(const unsigned char *input, const size_t input_length, char **output)
{
    if (input == NULL || input_length == 0)
    {
        *output = calloc(1, 1);
        return;
    }

    size_t output_length = input_length * 2 + 1;
    if ((*output = malloc(output_length)) == NULL)
        return;

    int xdest = 0;
    char *dest = *output;

    for (int i = 0; i < input_length; i++)
    {
        dest[xdest++] = BASE16_ALPHABET[input[i] >> 4];
        dest[xdest++] = BASE16_ALPHABET[input[i] & 0b00001111];
    }

    dest[xdest] = '\0';
}

size_t from_base16(const char *input, unsigned char **output)
{
    size_t input_length = (input == NULL) ? 0 : strlen(input);
    if (input_length == 0 || input_length % 2 > 0)
        return 0;

    size_t output_length = input_length / 2;
    if ((*output = malloc(output_length)) == NULL)
        return 0;

    int xdest = 0;
    unsigned char *dest = *output;

    for (int i = 0; i < input_length; i += 2)
    {
        uint8_t val1 = decode_base16(input[i]);
        uint8_t val2 = decode_base16(input[i + 1]);

        if (val1 == INVALID_CHARACTER || val2 == INVALID_CHARACTER)
        {
            free(*output);
            *output = NULL;
            return 0;
        }

        dest[xdest++] = (val1 << 4) | val2;
    }

    return xdest;
}

void to_base32(const unsigned char *input, const size_t input_length, char **output)
{
    if (input == NULL || input_length == 0)
    {
        *output = calloc(1, 1);
        return;
    }

    size_t output_length = (size_t) (ceil(((double) input_length * 8) / 5) + 5);
    if ((*output = malloc(output_length)) == NULL)
        return;

    char *dest = *output;
    int num_padding_chars = 0;
    size_t total_chars_written = 0;

    for (int i = 0; i < input_length; i += 5)
    {
        if (input_length < 5 || i > input_length - 5)
        {
            if (input_length % 5 == 1)
                num_padding_chars = 6;
            else if (input_length % 5 == 2)
                num_padding_chars = 4;
            else if (input_length % 5 == 3)
                num_padding_chars = 3;
            else if (input_length % 5 == 4)
                num_padding_chars = 1;
        }

        encode_base32_group(&input[i], &dest[total_chars_written], num_padding_chars);
        total_chars_written += 8;
    }

    dest[total_chars_written] = '\0';
}

size_t from_base32(const char *input, unsigned char **output)
{
    size_t input_length = (input == NULL) ? 0 : strlen(input);
    if (input_length == 0 || input_length % 8 > 0)
        return 0;

    size_t output_length = input_length * 5 / 8;
    if ((*output = malloc(output_length)) == NULL)
        return 0;

    unsigned char *dest = *output;
    int num_padding_chars = 0;
    size_t total_decoded_bytes = 0;

    for (int i = 0; i < input_length; i += 8)
    {
        if (i == input_length - 8)
        {
            if (input[i + 2] == '=')
                num_padding_chars = 6;
            else if (input[i + 4] == '=')
                num_padding_chars = 4;
            else if (input[i + 5] == '=')
                num_padding_chars = 3;
            else if (input[i + 7] == '=')
                num_padding_chars = 1;
        }

        size_t num_decoded_bytes = decode_base32_group(&input[i], &dest[total_decoded_bytes], num_padding_chars);
        if (num_decoded_bytes == 0)
        {
            free(*output);
            *output = NULL;
            return 0;
        }
        total_decoded_bytes += num_decoded_bytes;
    }

    return total_decoded_bytes;
}

void to_base64(const unsigned char *input, const size_t input_length, char **output)
{
    if (input == NULL || input_length == 0)
    {
        *output = calloc(1, 1);
        return;
    }

    size_t output_length = (size_t) (4 * (ceil((double) input_length / 3)) + 1);
    if ((*output = malloc(output_length)) == NULL)
        return;

    char *dest = *output;
    int num_padding_chars = 0;
    size_t total_chars_written = 0;

    for (int i = 0; i < input_length; i += 3)
    {
        if (input_length < 3 || i > input_length - 3)
        {
            if (input_length % 3 == 2)
                num_padding_chars = 1;
            else if (input_length % 3 == 1)
                num_padding_chars = 2;
        }

        encode_base64_group(&input[i], &dest[total_chars_written], num_padding_chars);
        total_chars_written += 4;
    }

    dest[total_chars_written] = '\0';
}

size_t from_base64(const char *input, unsigned char **output)
{
    size_t input_length = (input == NULL) ? 0 : strlen(input);
    if (input_length == 0 || input_length % 4 > 0)
        return 0;

    size_t output_length = input_length * 3 / 4;
    if ((*output = malloc(output_length)) == NULL)
        return 0;

    unsigned char *dest = *output;
    int num_padding_chars = 0;
    size_t total_decoded_bytes = 0;

    for (int i = 0; i < input_length; i += 4)
    {
        if (i == input_length - 4)
        {
            if (input[i + 2] == '=')
                num_padding_chars = 2;
            else if (input[i + 3] == '=')
                num_padding_chars = 1;
        }

        size_t num_decoded_bytes = decode_base64_group(&input[i], &dest[total_decoded_bytes], num_padding_chars);
        if (num_decoded_bytes == 0)
        {
            free(*output);
            *output = NULL;
            return 0;
        }
        total_decoded_bytes += num_decoded_bytes;
    }

    return total_decoded_bytes;
}
