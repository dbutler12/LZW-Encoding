#include <stdlib.h>
#include <bitStream.h>

// Sets aside memory for a read bit stream and initializes all the elements
BitStream* openInputBitStream(int (*readFunc)(void* context), void* context){
    BitStream* bitStream = malloc(sizeof(BitStream));
    bitStream->context = context;
    bitStream->readFunc = readFunc;
    bitStream->direction = 0; // Direction is 0 for read
    bitStream->extraCount = 0;
    bitStream->extraBits = 0;
    bitStream->byteCount = 0;
    return bitStream;
}

// Sets aside memory for a write bit stream and initializes all the elements
BitStream* openOutputBitStream(void (*writeFunc)(unsigned char c, void* context), void* context){
    BitStream* bitStream = malloc(sizeof(BitStream));
    bitStream->context = context;
    bitStream->writeFunc = writeFunc;
    bitStream->direction = 1; // Direction is 1 for write
    bitStream->extraCount = 0;
    bitStream->extraBits = 0;
    bitStream->byteCount = 0;
    return bitStream;
}

// flushes the final remaining bits of the buffer to write as a byte
void flushOutBits(BitStream *bs){
    if(bs->direction == 0 || bs->extraCount < 1){ // refuses to do it if not needed
        return;
    }
    
    bs->writeFunc(bs->extraBits << (8 - bs->extraCount), bs->context);
    bs->extraCount = 0;
}

// free the bitstream
void closeAndDeleteBitStream(BitStream* bs){ 
    free(bs);
}

// for the encoder to write its codes in bytes
// adds the bits to its buffer then writes each bit in 8 bit chunks (leftmost chunks)
void outputBits(BitStream* bs, unsigned int nBits, unsigned int code){
    unsigned int write;
    unsigned int eBits = bs->extraBits << nBits; // shift buffer to make room for new code
    bs->extraCount += nBits;
    eBits = eBits + code; // put code with buffer
    
    while(bs->extraCount > 7){ // while there are enough bits to write
        write = ((255 << (bs->extraCount - 8)) & eBits) >> (bs->extraCount - 8); // cut out the 8 bits from the far left
                                                                                // then shift those bits all the way to the right
        bs->writeFunc(write, bs->context);
        bs->extraCount -= 8;
    }
    
    if(bs->extraCount == 0){ // set the buffer for future use
        bs->extraBits = 0;
    }else{
        eBits = (eBits << (32 - bs->extraCount)); // shift off the written bits
        bs->extraBits = eBits >> (32 - bs->extraCount); // shift back to the far right to prepare for future use
    }
}

// for the decoder to read its codes in more than bytes
// uses a buffer to store excess bits after a read
bool readInBits(BitStream* bs, unsigned int nBits, unsigned int* code){
    
    while(bs->extraCount < nBits){ // while more needs to be read
        int check = bs->readFunc(bs->context);
        if(check == -1) return false; // end of file reached
        unsigned int temp = ((unsigned int) check) << (24-bs->extraCount); // shift read bits to appropriate spot in buffer (far left)
        bs->extraBits += temp; // add the read bits to the buffer
        bs->extraCount += 8;
    }
    
    int i = 0;
    unsigned int bitGrab = 0;
    while(i < nBits){
        bitGrab += (1 << (31 - i++)); // Set up grabber to and with extraBits to get the code
    }
    
    *code = (bs->extraBits & bitGrab) >> (32-nBits); // code shifted to the right to be usable by decoder
    bs->extraBits = bs->extraBits << nBits; // shift off the used bits from the buffer (saving the unused ones to the far left)
    bs->extraCount -= nBits;
    
    return true;
}
