#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

struct value_info {
    unsigned char value;
    size_t count;
};


size_t byte_compress(unsigned char *buff_ptr, size_t buff_size) {
    if (buff_size == 0) return 0;

    struct value_info value_array[buff_size];
    unsigned char prior;
    unsigned char current;

    size_t total_value_count = 1;
    prior = value_array[0].value = buff_ptr[0];
    value_array[0].count = 1;
    //First pass value analysis
    for (size_t i = 1, j = 0; i < buff_size; i++) {
        current = buff_ptr[i];

        if (current != prior ) {
            ++j;
            value_array[j].value = current;
            value_array[j].count = 1;
            ++total_value_count;
        } else {
            ++value_array[j].count;
        }

        prior = current;
    }

    size_t compressed_size = total_value_count * 2 + 1;
    //Second pass mem modification
    if (compressed_size < buff_size) {
        buff_ptr[0] = 0xFF;

        for (size_t i = 1, j = 0; i < compressed_size; i += 2, j++) {
            buff_ptr[i] = value_array[j].count;
            buff_ptr[i+1] = value_array[j].value;
        }

        return compressed_size;

    } else{
        return buff_size;
    }
}

int main(int argc, char **argv) {

/*
unsigned char data_ptr[] = {0x03, 0x74, 0x04, 0x04, 0x04, 0x35, 0x35, 0x64,
0x64, 0x64, 0x64, 0x00, 0x00, 0x00, 0x00, 0x00,
0x56, 0x45, 0x56, 0x56, 0x56, 0x09, 0x09, 0x09};

size_t data_size = 24;
*/
/**/
unsigned char data_ptr[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

size_t data_size = 11;

size_t new_size = byte_compress(data_ptr, data_size);

printf("\nFinal Array: ");

for (size_t i = 0; i < new_size; i++) {
        printf("%u ", data_ptr[i]);
}
printf("\nFinal size: %zu. This is ~%.2f percent reduction in size\n", new_size , (1 - (double) new_size/ (double) data_size) * 100);

}