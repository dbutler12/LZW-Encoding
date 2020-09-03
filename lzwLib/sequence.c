#include <stdlib.h>
#include <sequence.h>

// Hash function, concept originates from http://cseweb.ucsd.edu/~kube/cls/100/Lectures/lec16/lec16-15.html
unsigned int stringHash(const unsigned char *string, unsigned int hashSize, unsigned int length){
    if(hashSize == 0) return 0;
    
    unsigned int code = 0;
    unsigned int prime = 37;
    
    int i = 0;
    while(i < length){
        code = (code * prime + string[i++]); // similar to Horner's algorithm, polynomial of form (((s)*a+s)*a+s)
                                            // makes 'layers' of a polynomial using the characters from the string as the added constants
    }
    
    return code % hashSize; // mod the solution down to the hash size we are using
}

// Sets aside memory for a new sequence, then initializes all the elements
Sequence* newSequence(unsigned char firstByte, unsigned long long hashSize){
    Sequence *sequence = malloc(sizeof(Sequence));
    sequence->string = malloc(sizeof(unsigned char));
    *(sequence->string) = firstByte;
    sequence->code = firstByte;
    sequence->nextSequence = NULL;
    sequence->hash = 0;
    sequence->length = 1;
    if(hashSize != 0) sequence->hash = stringHash(sequence->string, hashSize, 1);
    return sequence;
}

// Frees the memory for the sequence and all sequences attached to it
void deleteSequence(Sequence* sequence) {
    if(sequence == NULL) return;
    free(sequence->string);
    
    if(sequence->nextSequence != NULL){
        Sequence *temp = sequence->nextSequence;
        while(temp != NULL){
            Sequence *del = temp;
            temp = temp->nextSequence;
            free(del->string);
            free(del);
        }
    }
    if(sequence != NULL) free(sequence);
}

// Takes a sequence and a character, sets aside memory for a new sequence
// Then concatenates the old sequence string with the new character for the new sequence string
Sequence* copySequenceAppend(Sequence* sequence, unsigned char addByte, unsigned long long hashSize){
    Sequence *appSeq = malloc(sizeof(Sequence));
    appSeq->string = malloc((sequence->length + 1)*sizeof(unsigned char));
    
    int i = 0;
    while(i < sequence->length){
        appSeq->string[i] = sequence->string[i];
        i++;
    }
    
    appSeq->string[i] = addByte;
    appSeq->length = sequence->length + 1;
    appSeq->code = 0;
    appSeq->nextSequence = NULL;
    appSeq->hash = stringHash(appSeq->string, hashSize, appSeq->length);
    
    return appSeq;
}

// Writes all the characters of a sequence (for decoding)
unsigned int outputSequence(Sequence* sequence,
                    void (*writeFunc)(unsigned char c, void* context), void* context) {
    
    if(sequence == NULL || sequence == 0){
        return 0;
    }

    int i = 0;
    while(i < sequence->length){
        writeFunc(sequence->string[i], context);
        i++;
    }

    return 1;
}

// Checks to see if two sequences are the same by comparing their strings character by character
bool identicalSequences(Sequence* a, Sequence* b) {
    
    if(a == NULL || b == NULL) return false;
    if(a->length != b->length) return false;
    
    int i = 0;
    
    while(i < a->length){
        if(a->string[i] != b->string[i]) return false;
        i++;
    }
    
    return true;
}
