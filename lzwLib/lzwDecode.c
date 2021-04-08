#include <sequence.h>
#include <bitStream.h>
#include <lzw.h>

bool lzwDecode(unsigned int bits, unsigned int maxBits,
               int  (*readFunc )(void* context),
               void (*writeFunc)(unsigned char c, void* context),
               void* context){
    
    if(bits < LZW_MINIMUM_BITS || maxBits > LZW_MAXIMUM_BITS || bits > maxBits){
        fprintf(stderr, "Decoding bits out of range (must be between 8 and 24, inclusive)\n");
        return 0;
    }
    
    if(maxBits == 8){
        fprintf(stderr, "Max bits of 8 bits means file is not encoded. No decoding necessary.\n");
        return 0;
    }
    
    unsigned long long maxNum = (1 << maxBits); // another simple test to see if we are entirely out of codes
    BitStream *reader = openInputBitStream(readFunc, context);
    Sequence** table = (Sequence **)malloc(maxNum * sizeof(Sequence*)); // Setup the table with maxNum
    
    unsigned int i = 0;
    while(i < maxNum) table[i++] = 0;
    unsigned int tIndex = 0; // keeps track of the next code
    while(tIndex < 256){
        table[tIndex] = newSequence(tIndex, 0);
        tIndex++;
    }
    
    unsigned int bitTest = 1 << bits; // a simple test to see if we are out of codes for our current block
    unsigned int prevCode, curCode;
    char c;
    Sequence* w;
    
    if(readInBits(reader, bits, &prevCode));
    unsigned int check = outputSequence(table[prevCode], writeFunc, context);
    if((tIndex & bitTest) != 0){ //Check if we need to increase code bit size
        bitTest = bitTest << 1;
        bits++;
    }
    
    while(readInBits(reader, bits, &curCode)){
        if(curCode > tIndex || check == 0) { // Close the program with an error
            check = 0;
            break;
        }
        if(table[curCode] != NULL){
            c = table[curCode]->string[0];
        }else{
            c = table[prevCode]->string[0];
        }
        
        if(tIndex < maxNum){ // Table not full
            w = copySequenceAppend(table[prevCode], c, 0);
            table[tIndex++] = w;
        }
        
        if(tIndex == bitTest && bits < maxBits){ //Check if we need to increase code bit size
            bitTest = bitTest << 1;
            bits++;
        }
        check = outputSequence(table[curCode], writeFunc, context); // print the sequence to writefile
        if(check == 0){ 
            break;
        }
        prevCode = curCode;
    }

    if(check == 0){
        fprintf(stderr, "Undefined code discovered. Encoding may have failed, or the wrong bit range was selected for this file.\n");
        i = 0;
        while(i < tIndex){
            if(table[i] != NULL) deleteSequence(table[i]);
            i++;
        }
        closeAndDeleteBitStream(reader);
        free(table);
        return 0;
    }

    i = 0;
    while(i < tIndex){ // free the table sequences before the table
        if(table[i] != NULL) deleteSequence(table[i]);
        i++;
    }
    closeAndDeleteBitStream(reader);
    free(table);
    
    return 1;  
}
