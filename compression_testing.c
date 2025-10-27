#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include "run_length_encoding.h"

struct test_case {
    char *case_name;
    unsigned char *buff_ptr;
    unsigned char *expected_compressed_buff;
    size_t buff_size;
    size_t buff_capacity;
};

//Test Case 1: (Assignment example)
unsigned char test1[] = {0x03, 0x74, 0x04, 0x04, 0x04, 0x35, 0x35, 0x64,
    0x64, 0x64, 0x64, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x56, 0x45, 0x56, 0x56, 0x56, 0x09, 0x09, 0x09};
//Test Case 2: Worst case scenario (No repeated values)
unsigned char test2[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};
//Test Case 3: Best case Scenario (All repeated values)
unsigned char test3[] = {0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F,
    0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F ,0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F};
//Test Case 4: No values
unsigned char test4[] = {};
//Test Case 5: Repeated Values, but compressed size w/ flag = original size
unsigned char test5[] = {0x00, 0x00, 0x00, 0x10, 0x10, 0x7F, 0x7F, 0x0F, 0x0F};
//Test Case 6: Buffer with a run length > 255 (more than a count byte can hold)
unsigned char test6[1000]; 
//memset(test6, 0x7F, sizeof test6); ARGHHHH ISSSUEEEEE.

int main(int argc, char **argv) {
    (void) argc;
    (void) argv;
    /*
    *Decided to treat as unsigned, since we are just treating them as raw bytes
    even if the specs specify 0-127
    * LIST ADVANTAGES/DESIGN REASON HERE
    */
    /**/

    //unsigned char data_ptr[1000]; 
    //memset(data_ptr, 0x7F, sizeof data_ptr);
    //size_t data_size = 1000;
    
    //unsigned char original_buffer[1000];
    //memset(original_buffer, 0x7F, sizeof original_buffer);

    unsigned char data_ptr[256]; 
    memset(data_ptr, 0x7F, sizeof data_ptr);
    size_t data_size = 256;

    unsigned char original_buffer[256];
    memset(original_buffer, 0x7F, sizeof original_buffer);

    printf("\nOriginal Buffer: [");

    for (size_t i = 0; i < data_size; i++) {
            printf("%u ", data_ptr[i]);
    }

    printf("]\n");
    printf("Original size: %zu bytes.\n", data_size);

    size_t new_size = byte_compress(data_ptr, data_size);

    printf("\nCOMPRESSION STARTING:\n");

    printf("Compressed Buffer: [");

    for (size_t i = 0; i < new_size; i++) {
            printf("%u ", data_ptr[i]);
    }
    printf("]\nCompressed size: %zu bytes. This is a ~%.2f percent reduction in size\n", new_size , (data_size ? (1 - (double) new_size / (double) data_size) : 0) * 100);


    printf("\nDECOMPRESSION STARTING:\n");

    size_t final_size = byte_decompress(data_ptr, new_size, 1001);

    printf("Decompressed Buffer: [");

    for (size_t i = 0; i < final_size; i++) {
            printf("%u ", data_ptr[i]);
    }
    printf("]\nDecompressed size: %zu bytes.\n", final_size);

    printf("\n");


    //Make sure both buffers are still equal after both operations
    if (final_size == data_size) {
        if (memcmp(original_buffer, data_ptr, final_size) == 0) printf("Same final size and bytes.\n");
        else printf("Different Final Bytes");
    }
    else printf("Different final sizes\n");
    //How to format accurately
    //printf("0x%02X\n", 0x0F);
}