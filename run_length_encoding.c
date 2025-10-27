#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>

/**
 * @brief Compress a byte buffer in place using run length encoding.
 * 
 * Description:
 * - Compressed format (if buffer is compressed): 
 *   [0] = 0xFF (flag for decompression), 
 *   [1] = count1 , [2] = value1, ... ((count, value) pairs for each run)
 * - The counts are 1 byte, run lengths greater than 255 will be split into 
 *   multiple (count, value) pairs.
 * - The size is of the format 2n + 1, n being the number of (count, value) 
 *   pairs
 * 
 * Conditions:
 * - The buff_ptr will point to an array of bytes of length buff_size.
 * - Each byte will contain a number from 0 to 127 (0x00 to 0x7F).
 * 
 * Result:
 * - If compressed_size < buff_size the buffer will be modified and the 
 *   return will be compressed_size
 * - If compressed_size >= buff_size the buffer will not be modified and the 
 *   return will be buff_size
 * - If an allocation error is encountered, the buffer will not be modified and *   the return will be buff_size
 * 
 * @param buff_ptr - A pointer to the input buffer that will be compressed
 * @param buff_size - The length of the input buffer
 * 
 * @return - The number of valid bytes in the buffer after function completion
 **/
size_t byte_compress(unsigned char *buff_ptr, size_t buff_size) {
    struct value_info {
    unsigned char value;
    size_t count;  
    };

    if (buff_ptr == NULL) return buff_size;
    if (buff_size < 1) return buff_size;

    struct value_info *value_array = malloc(buff_size * sizeof *value_array);
    if (!value_array) {
        return buff_size; 
    }

    unsigned char prior; 
    unsigned char current;

    prior = value_array[0].value = buff_ptr[0];
    value_array[0].count = 1;
    size_t total_value_count = 1;
    
    size_t max_value_count = 255;
    int max_count_flag = 0;

    for (size_t i = 1, j = 0; i < buff_size; i++) {
        current = buff_ptr[i];
        if (current != prior || max_count_flag) {
            if (max_count_flag) max_count_flag = 0;
            ++j;
            value_array[j].value = current;
            value_array[j].count = 1;
            ++total_value_count;

        } else {
            ++value_array[j].count;
        }

        if (value_array[j].count == max_value_count) {
            max_count_flag = 1;
        }
        prior = current;
    }

    size_t compressed_size = total_value_count * 2 + 1;

    if (compressed_size < buff_size) {
        buff_ptr[0] = 0xFF;
        for (size_t i = 1, j = 0; i < compressed_size; i += 2, j++) {
            buff_ptr[i] = value_array[j].count;
            buff_ptr[i+1] = value_array[j].value;
        }

        free(value_array);
        return compressed_size;

    } else{
        free(value_array);
        return buff_size;
    }
}

/**
 * @brief Decompress a byte buffer in place using run length encoding.
 * 
 * Description:
 * - Decompressed Format (if buffer is decompressed):
 *  - A array of bytes, each containing a number from 0 to 127 (0x00 to 0x7F).
 * 
 * Conditions:
 * - The buff_ptr will point to an array of bytes of length buff_size.
 * 
 * Result:
 * - If buff_size is not of the form 2n + 1, or there is no compressed flag at 
 *   the 0th indice, the buffer will not be modified and the return will be 
 *   buff_size
 * - If decompressed_size <= buff_capacity the buffer will be modified and the 
 *   return will be decompressed_size
 * - If decompressed_size > buff_capacity the buffer will not be modified and 
 *   the return will be buff_size
 * - If an allocation error is encountered, the buffer will not be modified and *   the return will be buff_size
 * 
 * @param buff_ptr - A pointer to the input buffer that will be decompressed
 * @param buff_size - The length of the input buffer
 * @param buff_size - The capacity allocated to the buff_ptr
 * 
 * @return - The number of valid bytes in the buffer after function completion
 **/
size_t byte_decompress(unsigned char *buff_ptr, size_t buff_size, size_t buff_capacity) {
    struct value_info {
    unsigned char value;
    size_t count;
    };

    if (buff_ptr == NULL) return buff_size;
    if (buff_size < 1 || (buff_size - 1) % 2 != 0 ) return buff_size;

    if (buff_ptr[0] != 0xFF) {
        return buff_size;
    }

    size_t total_value_count = (buff_size -1)/2;
    struct value_info *value_array = malloc(total_value_count * sizeof *value_array);
    if (!value_array) { 
        return buff_size; 
    }
    for (size_t i = 1, j = 0; i < buff_size; i += 2, j++) {
        value_array[j].count = buff_ptr[i];
        value_array[j].value = buff_ptr[i+1];
    }

    size_t decompressed_size = 0;
    for (size_t j = 0; j < total_value_count; j++) {
        decompressed_size += value_array[j].count;
    }

    if (buff_capacity < decompressed_size) {
        free(value_array);
        return buff_size;
    }

    size_t count;
    unsigned char value;
    size_t i = 0;
    for (size_t j = 0; j < total_value_count; j++) {
        count = value_array[j].count;
        value = value_array[j].value;
        for (size_t k = 0; k < count; i++, k++) {
            buff_ptr[i] = value;
        }   
    }
    free(value_array);
    return decompressed_size;
}

