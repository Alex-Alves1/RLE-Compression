#include <stddef.h>

#ifndef RUN_LENGTH_ENCODING_H
#define RUN_LENGTH_ENCODING_H

size_t byte_compress(unsigned char *buff_ptr, size_t buff_size);

size_t byte_decompress(unsigned char *buff_ptr, size_t buff_size, size_t buff_capacity);

#endif