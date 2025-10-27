#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>

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

/**
 * @brief Compress a byte buffer in place using run length encoding.
 * 
 * Description:
 *  This function will receive an input buffer of size buff_size, will do a
 * pass over the buffer to record value and count pairs for each run, and
 * then if it calculates that the compressed size would be smaller than the 
 * original, will overwrite the input buffer with the compressed version and 
 * return the new buffer length. It will also place a 0xFF flag at the first 
 * element of the compressed buffer as an indicator to the decompression 
 * algorithm that it can be decompressed. 
 *  The format of the compressed buffer if succesful will be [0] = 0xFF, [1] = 
 * count1, [2] = value1, [3] = count2, [4] = value2, ...
 * This means that the size of the compressed buffer will always be 2n + 1 
 * where n is the number of runs in the input buffer.
 * 
 * Conditions:
 * - The data_ptr will point to an array of bytes of length buff_size.
 * - Each byte will contain a number from 0 to 127 (0x00 to 0x7F).
 * 
 * Result:
 * - If compressed_size < buff_size the buffer will be modified and the 
 *   return will be compressed_size
 * - If compressed_size >= buff_size the buffer will not be modified and the 
 *   return will be buff_size
 * - If an allocation error is encountered, the buffer will not be modified and *   the return will be buff_size
 * 
 * @param buff_ptr - A pointer to the buffer that will be compressed
 * @param buff_size - The length of the input buffer
 * 
 * @return - The number of valid bytes in the buffer after function completion
 **/
size_t byte_compress(unsigned char *buff_ptr, size_t buff_size) {
    int debug = 0;
    // Handle the case where an empty buffer is passed instantly
    if (buff_size < 1) return buff_size;
    //Read through each byte, mark prior, look at future, if same, look at 
    //future again. If not same then store that count and modify the buffer. 
    //Then make sure we modify the index to match where new data will be
    
    //Initializing this array on the stack instead of heap with malloc, faster 
    //and small buffers shouldnt take up too much space.

    //actually ended up going with malloc for safety, but can list as a potential improvement to allocate onto stack if buffer sizing is guaranteed to be small.
    struct value_info *value_array = malloc(buff_size * sizeof *value_array);
    if (!value_array) { 
        perror("malloc"); 
        return buff_size; 
    }
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

    //max value count dictates the number of instances of a value that can be 
    //grouped, this will be limited to 255, the max a byte can display.
    size_t max_value_count = 255;
    int max_count_flag = 0;
    for (size_t i = 1, j = 0; i < buff_size; i++) {
        current = buff_ptr[i];
        if (current != prior || max_count_flag) {
            max_count_flag = 0;
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
        //To check if we are about to overflow the count, since a byte is 
        //limited to 255
        if (value_array[j].count == max_value_count) {
            max_count_flag = 1;
        }
        prior = current; //So we can still reference prior up until here
    }

    for (size_t i = 0; i < total_value_count; i++) {
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
        free(value_array);
        return compressed_size;
    } else{
        free(value_array);
        return buff_size;
    }
}

//Should the decompression modify the original memory back? This shouldnt result in any errors since it would decompress back to OG size. It just seems weird because we would be accessing memory past where the current buffer size is.
//returns -1 if the buffer capacity isnt enough to accomodate the decompressed //size
size_t byte_decompress(unsigned char *buff_ptr, size_t buff_size, size_t buff_capacity) {
    int debug = 0;
    //Checking to make sure its not an empty buffer, or its not sized 
    //incorrectly (2n + 1 sizing is what the compression algorithm outputs)
    if (buff_size < 1 || (buff_size - 1) % 2 != 0 ) {
        return buff_size;
    }
    if (buff_ptr[0] != 0xFF) { //The flag is missing so it was never compressed
        printf("This is not a properly compressed buffer.");
        return buff_size;
    }
    //Check first if it is marked as compressed
    if (debug) printf("HERE IN PHASE 1 OF DECOMPRESSION\n");
    //The value struct of the uncompressed list will be half the amount of the actual values - 1 for the flag
    size_t total_value_count = (buff_size -1)/2;
    if (debug) printf("SIZE OF NEW ALLOCATED ARRAY: %zu\n",total_value_count);
    struct value_info *value_array = malloc(total_value_count * sizeof *value_array);
    if (!value_array) { 
        perror("malloc"); 
        return buff_size; 
    }
    //starting at 1 to avoid the flag
    for (size_t i = 1, j = 0; i < buff_size; i += 2, j++) {
        value_array[j].count = buff_ptr[i];
        value_array[j].value = buff_ptr[i+1];
        if (debug) printf("HERE IS VALUE %u\n",value_array[j].value);
    }

    for (size_t i = 0; i < total_value_count; i++) {
    if (debug) printf("Value: %u , Count: %zu\n", value_array[i].value, value_array[i].count);
    }

    //Checking to make sure the decempressed size would be smaller or equal to the buffer capacity function parameter before modifying memory.
    size_t decompressed_buff_size = 0; //Should always be equal to initial
    for (size_t j = 0; j < total_value_count; j++) {
        decompressed_buff_size += value_array[j].count;
    }

    //Buffer capacity is too low to unpack the compressed buffer, return before 
    //mem modification
    if (buff_capacity < decompressed_buff_size) {
        free(value_array);
        return buff_size;
    }

    //Second pass to overwrite memory
    size_t count;
    unsigned char value;
    size_t i = 0;
    for (size_t j = 0; j < total_value_count; j++) {
        if (debug) printf("RUNNING THE PLACEMENTS HERE: \n");
        count = value_array[j].count;
        value = value_array[j].value;
        //I is buffer indice, k is count indice
        for (size_t k = 0; k < count; i++, k++) {
            //THIS IS THE BUG, CONSTANTLY WRITING OVER THE SAME DATA.
            buff_ptr[i] = value;
            if (debug) printf("Running sub placement here: %u \n", buff_ptr[i]);
        }   
    }
    free(value_array);
    return decompressed_buff_size;
}

