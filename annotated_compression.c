#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

//Write in 1 or 2 files?

//This is the struct we will record the info of each item into the buffer into, 
//this is so we can make a first pass and find how many different values there 
//are and their corresponding count. We can place these into an array with max 
//size of buffer.

//This will let us see the count in the struct array, and determine whether 
//compression is worth it and whether we should overwrite the buffer or not. //
//Also makes overwriting the buffer easier since we dont have to worry about 
//overwriting "unread" values as we would with a one pass implementation and 
//already have access to values ahead of the current iteration.

struct value_info {
    unsigned char value;
    size_t count;
};

//Will be called with two args, data_ptr, data_size
//Will return the size of the new data set
size_t byte_compress(unsigned char *buff_ptr, size_t buff_size) {
    int debug = 0;
    // Handle the case where an empty buffer is passed instantly
    if (buff_size == 0) return 0;
    //Read through each byte, mark prior, look at future, if same, look at 
    //future again. If not same then store that count and modify the buffer. 
    //Then make sure we modify the index to match where new data will be
    
    //Initializing this array on the stack instead of heap with malloc, faster 
    //and small buffers shouldnt take up too much space.
    struct value_info value_array[buff_size];

    //Need to make sure we dont overwrite
    //j is to track current position in the value array, i is to track current 
    //position in the buffer
    //FIRST PASS, JUST TO LOAD ALL INTO VALUE ARRAY

    //Initialize first group including initializing count, first value in buffer
    //and the prior

     //This will mark the current byte value. MAY NOT NEED if can
    //we just compare like: buff_ptr[i] == buff_ptr[i+1] each time.
    unsigned char prior; //Initializing to this since the buffer will never have this value as an item. This will also be the flag at the beginning of the compressed buffer.
    unsigned char current;

    prior = value_array[0].value = buff_ptr[0];
    value_array[0].count = 1;
    //Will track total number of values entered into the value array
    size_t total_value_count = 1;

    //Starting from i = 1 since weve already input the first value into the 
    //value array
    for (size_t i = 1, j = 0; i < buff_size; i++) {
        current = buff_ptr[i];
        if (current != prior ) {
            ++j; //Only incrementing J after having entered this if condition
            //setting initial value in the value arrays for the new value
            value_array[j].value = current;
            value_array[j].count = 1; //This defines that it appears for the first time
            ++total_value_count;

            //if current == prior means that we went to the next value and 
            //it was the same
        } else {
            ++value_array[j].count;
        }

        prior = current; //So we can still reference prior up until here
    }

    for (int i = 0; i < total_value_count; i++) {
        if (debug) printf("Value: %u , Count: %zu\n", value_array[i].value, value_array[i].count);
    }

    
    if (debug) printf("Final Value Array Count: %zu\n", total_value_count);
    //final compressed size should be double the number of values recorded in 
    //the array ({count, value} pair for each one) as well as 1 for the flag. 
    //MAKE SURE TO VERIFY THIS IS ACCURATE AND WE DONT DO THIS FOR NON 
    //COMPRESSED BUFFERS
    size_t compressed_size = total_value_count * 2 + 1;

    //Design decision to just return default array if the compressed version is 
    //of equal length
    //If it is worth it then we will do the memory swap, if not we return the 
    //inputted size
    if (compressed_size < buff_size) {
        //Set first element as 0xFF. This is the flag for a compressed buffer 
        //that the decompressor can recognize since buffer is limited to 0x7F
        buff_ptr[0] = 0xFF;
        //Iterating i by 2 each time since the value arrays contain both count
        //and the value itself
        for (size_t i = 1, j = 0; i < compressed_size; i += 2, j++) {
            buff_ptr[i] = value_array[j].count;
            buff_ptr[i+1] = value_array[j].value;
        }
        return compressed_size;
    } else{
        return buff_size;
    }
}

//Should the decompression modify the original memory back? This shouldnt result in any errors since it would decompress back to OG size. It just seems weird because we would be accessing memory past where the current buffer size is.
size_t byte_decompress(unsigned char *buff_ptr, size_t buff_size) {
    int debug = 0;
    //Check first if it is marked as compressed
    if (buff_ptr[0] == 0xFF) {
        if (debug) printf("HERE IN PHASE 1 OF DECOMPRESSION\n");
        //The value struct of the uncompressed list will be half the amount of the actual values - 1 for the flag
        size_t total_value_count = (buff_size -1)/2; //
        if (debug) printf("SIZE OF NEW ALLOCATED ARRAY: %zu\n",total_value_count);
        struct value_info v_array[total_value_count]; //
        //starting at 1 to avoid the flag
        for (size_t i = 1, j = 0; i < buff_size; i += 2, j++) {
            v_array[j].count = buff_ptr[i];
            v_array[j].value = buff_ptr[i+1];
            if (debug) printf("HERE IS VALUE %u\n",v_array[j].value);
        }

        for (int i = 0; i < total_value_count; i++) {
        if (debug) printf("Value: %u , Count: %zu\n", v_array[i].value, v_array[i].count);
        }

        //Second pass
        size_t decompressed_buff_size = 0; //Should always be equal to initial
        size_t count;
        unsigned char value;
        size_t i = 0;
        for (size_t j = 0; j < total_value_count; j++) {
            if (debug) printf("RUNNING THE PLACEMENTS HERE: \n");
            count = v_array[j].count;
            value = v_array[j].value;
            //I is buffer indice, k is count indice
            for (size_t k = 0; k < count; i++, k++) {
                //THIS IS THE BUG, CONSTANTLY WRITING OVER THE SAME DATA.
                buff_ptr[i] = value;
                if (debug) printf("Running sub placement here: %u \n", buff_ptr[i]);
                ++decompressed_buff_size;
            }   
        }
        return decompressed_buff_size;
        //run decompression
        //Should I run a one pass or two pass algo here? I could use the 
        //structs again, or I could just strdup and read off the dup.

    } else { //The flag is missing so it was never compressed
        return buff_size;
    }
}

int main(int argc, char **argv) {
    /*
    *Decided to treat as unsigned, since we are just treating them as raw bytes
    even if the specs specify 0-127
    * LIST ADVANTAGES/DESIGN REASON HERE
    */
    /**/
    unsigned char data_ptr[] = {0x03, 0x74, 0x04, 0x04, 0x04, 0x35, 0x35, 0x64,
    0x64, 0x64, 0x64, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x56, 0x45, 0x56, 0x56, 0x56, 0x09, 0x09, 0x09};

    size_t data_size = 24;
    
    /*
    unsigned char data_ptr[] = {0x00, 0x00, 0x33, 0x33, 0x33, 0x33, 0x33, 0x00, 0x00, 0x00, 0x00};

    size_t data_size = 11;
    */
    printf("\nOriginal Buffer: [");

    for (size_t i = 0; i < data_size; i++) {
            printf("%u ", data_ptr[i]);
    }

    printf("]\n");
    printf("Compressed size: %zu.\n", data_size);

    size_t new_size = byte_compress(data_ptr, data_size);

    printf("\nCOMPRESSION STARTING:\n");

    printf("Compressed Buffer: [");

    for (size_t i = 0; i < new_size; i++) {
            printf("%u ", data_ptr[i]);
    }
    printf("]\nCompressed size: %zu. This is ~%.2f percent reduction in size\n", new_size , (1 - (double) new_size/ (double) data_size) * 100);

    printf("\nDECOMPRESSION STARTING:\n");

    size_t final_size = byte_decompress(data_ptr, new_size);

    printf("Decompressed Buffer: [");

    for (size_t i = 0; i < final_size; i++) {
            printf("%u ", data_ptr[i]);
    }

    printf("]\nDecompressed size: %zu. This is ~%.2f percent reduction in size\n", final_size , (1 - (double) final_size/ (double) data_size) * 100);

    printf("\n");
}