#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>

/* Function Declarations from run_length_encoding */
extern size_t byte_compress(
    unsigned char *buff_ptr, 
    size_t buff_size
);

extern size_t byte_decompress(
    unsigned char *buff_ptr, 
    size_t buff_size, 
    size_t buff_capacity
);

struct test_case {
    const char *case_name;
    const unsigned char *buff_ptr;
    size_t buff_size;
    const unsigned char *expected_compressed_buff;
    size_t expected_buff_size;
    size_t buff_capacity;
};

int main(int argc, char **argv) {
    (void) argc;
    (void) argv;

    // Test Case 1: (Assignment example, compresses)
    unsigned char test1[] = {0x03, 0x74, 0x04, 0x04, 0x04, 0x35, 0x35, 0x64,
        0x64, 0x64, 0x64, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x56, 0x45, 0x56, 0x56, 0x56, 0x09, 0x09, 0x09};
    unsigned char expected_test1[] = {0xFF, 0x01, 0x03, 0x01, 0x74, 0x03, 0x04, 
        0x02, 0x35, 0x04, 0x64, 0x05, 0x00, 0x01, 0x56, 0x01, 0x45, 0x03, 0x56, 
        0x03, 0x09};
    // Test Case 2: Worst case scenario (No repeated values, doesnt compress)
    unsigned char test2[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};
    unsigned char expected_test2[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 
        0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};
    // Test Case 3: Best case Scenario (All repeated values, compresses)
    unsigned char test3[] = {0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F,
        0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F};
    unsigned char expected_test3[] = {0xFF, 0x10, 0x7F};
    // Test Case 4: Empty buffer
    unsigned char *test4 = NULL;
    unsigned char *expected_test4 = NULL;
    // Test Case 5: Repeated Values, but compressed size w/ flag = original 
    // size (doesnt compress)
    unsigned char test5[] = {0x00, 0x00, 0x00, 0x10, 0x10, 0x7F, 0x7F, 0x0F,    
        0x0F};
    unsigned char expected_test5[] = {0x00, 0x00, 0x00, 0x10, 0x10, 0x7F, 0x7F, 
        0x0F, 0x0F};
    // Test Case 6: Buffer with a run length > 255 (more than a count byte can 
    // hold, compresses)
    unsigned char test6[1000] = { [0 ... 999] = 0x7F }; 
    unsigned char expected_test6[] = {0xFF, 0xFF, 0x7F, 0xFF, 0x7F, 0xFF, 0x7F, 
        0xEB, 0x7F};
    // Test Case 7: lower limit before splitting run lengths
    unsigned char test7[255] = { [0 ... 254] = 0x0A}; 
    unsigned char expected_test7[] = {0xFF, 0xFF, 0x0A};
    // Test Case 8: Right after splitting run lengths
    unsigned char test8[256]; 
    memset(test8, 0x70, sizeof test8);
    unsigned char expected_test8[] = {0xFF, 0xFF, 0x70, 0x01, 0x70};
    // Test Case 9: Testing compression with varying and repeating run lengths
    unsigned char test9[] = { 0x00, 0x00, 0x00, 0x63, 0x07, 0x07, 0x40, 
        0x40, 0x40, 0x40, 0x07, 0x07, 0x00, 0x00, 0x00, 0x00};
    unsigned char expected_test9[] = {0xFF, 0x03, 0x00, 0x01, 0x63, 0x02, 0x07, 
        0x04, 0x40, 0x02, 0x07, 0x04, 0x00};
    // Test Case 10: Single byte. not worth compressing.
    unsigned char test10[] = {0x01};
    unsigned char expected_test10[] = {0x01};
    // Test Case 11: 2 bytes of same value. Not worth compressing.
    unsigned char test11[] = {0x02, 0x02};
    unsigned char expected_test11[] = {0x02, 0x02};
    // Test Case 12: 3 bytes of same value. Equivalent size to compress 
    // (because of flag).
    unsigned char test12[] = {0x03, 0x03, 0x03};
    unsigned char expected_test12[] = {0x03, 0x03, 0x03};
    // Test Case 13: 4 bytes of same value. Worth compressing.
    unsigned char test13[] = {0x04, 0x04, 0x04, 0x04};
    unsigned char expected_test13[] = {0xFF, 0x04, 0x04};
    // Test Case 14: 4 bytes of same value, 2 bytes of same value. Worth 
    // compressing.
    unsigned char test14[] = {0x04, 0x04, 0x04, 0x04, 0x02, 0x02};
    unsigned char expected_test14[] = {0xFF, 0x04, 0x04, 0x02, 0x02};
    // Test Case 15: Alternating bytes. Not worth compressing.
    unsigned char test15[] = {0x04, 0x02, 0x04, 0x02, 0x04, 0x02};
    unsigned char expected_test15[] = {0x04, 0x02, 0x04, 0x02, 0x04, 0x02};
    // Test Case 16: Large buffer with varying and repeating run lengths.
    unsigned char test16[4000] = {[0] = 0x00, [1 ... 200] = 0x7F, 
        [201 ... 573] = 0x0A, [574 ... 583] = 0x70, [584 ... 883] = 0x7F, 
        [884 ... 885] = 0x02, [886 ... 3999] = 0x0F} ;
    unsigned char expected_test16[] = {0xFF, 0x01, 0x00, 0xC8, 0x7F, 0xFF, 
        0x0A, 0x76, 0x0A, 0x0A, 0x70, 0xFF, 0x7F, 0x2D, 0x7F, 0x02, 0x02, 
        0xFF, 0x0F, 0xFF, 0x0F, 0xFF, 0x0F, 0xFF, 0x0F, 0xFF, 0x0F, 0xFF, 0x0F,
        0xFF, 0x0F, 0xFF, 0x0F, 0xFF, 0x0F, 0xFF, 0x0F, 0xFF, 0x0F, 0xFF, 0x0F, 
        0x36, 0x0F};  

    struct test_case cases[] = {
        {"Case 1: Given assignment example.\nCompresses.", test1, sizeof test1, 
            expected_test1, sizeof expected_test1, sizeof test1},

        {"Case 2: 16 Byte RLE worst case scenario.\nDoes not compress.", test2, 
            sizeof test2, expected_test2, sizeof expected_test2, sizeof test2},

        {"Case 3: 16 byte RLE best case Scenario.\nCompresses.", test3, 
            sizeof test3, expected_test3, sizeof expected_test3, sizeof test3},

        {"Case 4: Empty buffer.\nDoes not compress.", test4, 0, 
            expected_test4, 0, 0},

        {"Case 5: 9 bytes mixed run lengths, compressed size = original size."
            "\nDoes not compress.", test5, sizeof test5, expected_test5, 
            sizeof expected_test5, sizeof test5},

        {"Case 6: Run length >> count limit (1000).\nCompresses.", test6, sizeof 
            test6, expected_test6, sizeof expected_test6, sizeof test6},

        {"Case 7: Run length at boundary of count limit (255).\nCompresses.", 
            test7, sizeof test7, expected_test7, sizeof expected_test7, 
            sizeof test7},

        {"Case 8: Run length above boundary of count limit (256).\nCompresses.", 
            test8, sizeof test8, expected_test8, sizeof expected_test8, 
            sizeof test8},

        {"Case 9: 16 bytes Varying and repeating run lengths. Compresses" 
            "\nPassed a smaller than original capacity during decompression."
            "\nIt will not decompress the compressed buffer.", test9, sizeof 
            test9, expected_test9, sizeof expected_test9, 5},

        {"Case 10: Single byte.\nDoes not compress.", test10, sizeof test10, 
            expected_test10, sizeof expected_test10, sizeof test10},

        {"Case 11: Two duplicate bytes.\nDoes not compress.", test11, sizeof 
            test11, expected_test11, sizeof expected_test11, sizeof test11},

        {"Case 12: Three of the same byte.\nDoes not compress.", test12, sizeof 
            test12, expected_test12, sizeof expected_test12, sizeof test12},

        {"Case 13: Four of the same byte.\nCompresses.", test13, sizeof test13, 
            expected_test13, sizeof expected_test13, sizeof test13},

        {"Case 14: Four of the same and two of the same bytes.\nCompresses.", 
            test14, sizeof test14, expected_test14, sizeof expected_test14, 
            sizeof test14},

        {"Case 15: Alternating single run bytes.\nDoes not compress.", test15, 
            sizeof test15, expected_test15, sizeof expected_test15, 
            sizeof test15},
        
        {"Case 16: Large buffer (4000) with varying large run lengths."
            "\nCompresses.", test16, sizeof test16, expected_test16, 
            sizeof expected_test16, sizeof test16},
    };

    size_t test_case_count = sizeof cases / sizeof cases[0];
    size_t new_size;
    size_t final_size;
    for (size_t i = 0; i < test_case_count; i++) {
        printf("%s\n", cases[i].case_name);
        printf("----------------------------------"
            "----------------------------------\n");

        size_t data_size = cases[i].buff_size;
        unsigned char buff_copy[(data_size > 0) ? data_size : 1];

        if (data_size) {
            memcpy(buff_copy, cases[i].buff_ptr, data_size);
        }

        new_size = byte_compress(buff_copy, data_size);
        printf("Post compressor: %zu bytes. This is a %.2f percent reduction"
            " in size.\n", new_size, 
            (cases[i].buff_size ? (1 - (double) new_size /
            (double) cases[i].buff_size) : 0) * 100);
        if (memcmp(buff_copy, cases[i].expected_compressed_buff, 
                new_size) == 0) {
            printf("Post compressor buffer matches the expected values.\n");
        }
        else {
            printf("Post compressor buffer does not match expected values.\n");
        }

        final_size = byte_decompress(buff_copy, new_size, cases[i].buff_capacity);
        printf("Post decompressor: %zu bytes.\n",final_size);

        if (final_size == data_size && 
            memcmp(buff_copy, cases[i].buff_ptr, data_size) == 0) {
            printf("Post decompressor buffer matches the original buffer.\n");
        } else if (final_size == new_size && 
            memcmp(buff_copy, cases[i].expected_compressed_buff, 
                new_size) == 0) {
            printf("Post decompressor buffer matches the compressed buffer.\n");
        } else {
            printf("Post decompressor buffer does not match expected values\n");
        }
        printf("\n");
    }
}