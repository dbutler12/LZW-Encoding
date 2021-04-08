#include <sequence.h>
#include <bitStream.h>
#include <dict.h>
#include <lzw.h>

bool lzwEncode(unsigned int bits, unsigned int maxBits,
               int (*readFunc)(void* context),
               void (*writeFunc)(unsigned char c, void* context),
               void *context){

    if(bits < LZW_MINIMUM_BITS || maxBits > LZW_MAXIMUM_BITS || bits > maxBits){
        fprintf(stderr, "Encoding bits out of range (must be between 8 and 24, inclusive)\n");
        return 0;
    }
    
    int read;
    
    if(maxBits == LZW_MINIMUM_BITS){
        fprintf(stderr, "No encoding occurs with 8 maximum bits. Program will run, but the file it produces will not be encoded.\n");
    }
    
    BitStream *writer = openOutputBitStream(writeFunc, context);
    Dict *dict = newDict(20971529); // Hash size chosen to be a prime number at least 1.25x the size of 2^24 (max possible bits)

    unsigned int bitTest = 1 << bits;     // a simple test to see if we are out of codes for our current block
    unsigned int maxCode = 1 << maxBits; // another simple test to see if we are entirely out of codes
    unsigned int code;
    bool freeWord = 0;   // used to keep track of if w needs to be freed or not
    bool wFromDict = 0; // used to keep track of if w is in the dictionary or not
    unsigned char c;
    
    read = readFunc(context);
    c = (unsigned char) read;
    Sequence* w = getCharFromDict(dict, c);
    Sequence* newWord;
    
    read = readFunc(context); // initial setup grabs two characters to begin
    while(read != EOF){
        c = (unsigned char) read;
        newWord = copySequenceAppend(w, c, dict->hashSize);
        if(searchDict(dict, newWord, &code)){ // Check if newWord is in dictionary
            if(freeWord == 1){ // freeWord = 1 means we are still using the sequence w
                Sequence *delSeq = w;
                if(delSeq!= NULL) deleteSequence(delSeq);
            }
            w = newWord;
            wFromDict = 0;
            freeWord = 1;
        }else{
            searchDict(dict, w, &code); // Gives code for w
            outputBits(writer, bits, code);
            if(freeWord == 1){
                Sequence *delSeq = w;
                if(delSeq!= NULL) deleteSequence(delSeq);
                freeWord = 0; // sets freeWord to 0 after deletion
            }
            if(dict->nextCode == bitTest && bits < maxBits){ //Check if we need to increase code bit size
                bitTest = bitTest << 1;
                bits++;
            }
            
            if(dict->nextCode < maxCode){
                insertDict(dict, newWord, dict->nextCode++); // Insert new word into dict if there is room
            }else{
                Sequence *delSeq = newWord;
                if(delSeq!= NULL) deleteSequence(delSeq);
            }
            
            w = getCharFromDict(dict, c);
            wFromDict = 1;
        }
        
        read = readFunc(context);
    }
    
    searchDict(dict,w,&code);
    if(!wFromDict) deleteSequence(w); // wFromDict prevents double free
    outputBits(writer, bits, code);
    deleteDictDeep(dict);
    flushOutBits(writer);
    closeAndDeleteBitStream(writer);

    return 1;
}
