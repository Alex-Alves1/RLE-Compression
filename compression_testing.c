#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include "run_length_encoding.h"

struct test_case {
    char *case_name;
    unsigned char *buff_ptr;
    size_t buff_size;
    unsigned char *expected_compressed_buff;
    size_t expected_buff_size;
    size_t buff_capacity;
};

int main(int argc, char **argv) {
    (void) argc;
    (void) argv;

    //Look into Linters

    // Test Case 1: (Assignment example, compresses)
    unsigned char test1[] = {0x03, 0x74, 0x04, 0x04, 0x04, 0x35, 0x35, 0x64,
        0x64, 0x64, 0x64, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x56, 0x45, 0x56, 0x56, 0x56, 0x09, 0x09, 0x09};
    unsigned char expected_test1[] = {0xFF, 0x01, 0x03, 0x01, 0x74, 0x03, 0x04, 
        0x02, 0x35, 0x04, 0x64, 0x05, 0x00, 0x01, 0x56, 0x01, 0x45, 0x03, 0x56, 
        0x03, 0x56};
    // Test Case 2: Worst case scenario (No repeated values, doesnt compress)
    unsigned char test2[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};
    unsigned char expected_test2[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 
        0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};
    // Test Case 3: Best case Scenario (All repeated values, compresses)
    unsigned char test3[] = {0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F,
        0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F};
    unsigned char expected_test3[] = {0xFF, 0x0F, 0x7F};
    // Test Case 4: Empty buffer
    unsigned char test4[] = {};
    unsigned char expected_test4[] = {};
    // Test Case 5: Repeated Values, but compressed size w/ flag = original size (doesnt compress)
    unsigned char test5[] = {0x00, 0x00, 0x00, 0x10, 0x10, 0x7F, 0x7F, 0x0F,    
        0x0F};
    unsigned char expected_test5[] = {0x00, 0x00, 0x00, 0x10, 0x10, 0x7F, 0x7F, 
        0x0F, 0x0F};
    // Test Case 6: Buffer with a run length > 255 (more than a count byte can 
    // hold, compresses)
    unsigned char test6[1000]; 
    memset(test6, 0x7F, sizeof test6);
    unsigned char expected_test6[] = {0xFF, 0xFF, 0x7F, 0xFF, 0x7F, 0xFF, 0x7F, 
        0xEB, 0x7F};
    // Test Case 7: lower limit before splitting run lengths
    unsigned char test7[255]; 
    memset(test7, 0x0A, sizeof test7);
    unsigned char expected_test7[] = {0xFF, 0xFF, 0x0A};
    // Test Case 8: Right after splitting run lengths
    unsigned char test8[256]; 
    memset(test8, 0x70, sizeof test8);
    unsigned char expected_test8[] = {0xFF, 0xFF, 0x70, 0x01, 0x70};
    // Test Case 9: Testing decompression if capacity < decompressed size
    
    size_t test_case_count = 8;
    struct test_case cases[] = {
        {"Case 1: Given Assignment Example. Compresses.", test1, sizeof test1, 
            expected_test1, sizeof expected_test1, sizeof test1},
        {"Case 2: Worst Case Scenario. Does not compress.", test2, sizeof 
            test2, expected_test2, sizeof expected_test2, sizeof test2},
        {"Case 3: Best Case Scenario. Compresses.", test3, sizeof test3, 
            expected_test3, sizeof expected_test3, sizeof test3},
        {"Case 4: Empty Buffer. Does not compress.", test4, sizeof test4, 
            expected_test4, sizeof expected_test4, sizeof test4},
        {"Case 5: Compression would result in same size. Does not compress.", 
            test5, sizeof test5, expected_test5, sizeof expected_test5, 
            sizeof test5},
        {"Case 6: Run length >> 255. Compresses.", test6, sizeof test6, 
            expected_test6, sizeof expected_test6, sizeof test6},
        {"Case 7: Run length at boundary of count limit (255). Compresses.", 
            test7, sizeof test7, expected_test7, sizeof expected_test7, 
            sizeof test7},
        {"Case 8: Run length above boundary of count limit (256). Compresses.", 
            test8, sizeof test8, expected_test8, sizeof expected_test8, 
            sizeof test8},
        //{"case 9", test9, sizeof test9, expected_test9, sizeof test1},
    };

    size_t new_size;
    size_t final_size;
    for (size_t i = 0; i < test_case_count; i++) {
        //How to make copies here without dynamic allocation, can I call strdup 
        //multiple times with same var?
        printf("%s\n", cases[i].case_name);
        new_size = byte_compress(cases[i].buff_ptr, cases[i].buff_size);
        printf("Compressed to: %zu bytes. This is a ~%.2f percent reduction in size.\n", new_size, (cases[i].buff_size ? (1 - (double) new_size / (double) cases[i].buff_size) : 0) * 100);
        if (memcmp(cases[i].buff_ptr, cases[i].expected_compressed_buff, 
            new_size) == 0) {
            printf("Compressed buffer matches expected outcome.\n");
        }
        final_size = byte_decompress(cases[i].buff_ptr, new_size, cases[i].buff_capacity);
        printf("Decompressed to: %zu bytes.\n\n",final_size);
    }
    //Loop through struct, call compress, compare to expected, call 
    //decompressed, compare to stringduped copy



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
    /*
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
    */
}