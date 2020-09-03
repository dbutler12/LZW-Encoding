#include <stdlib.h>
#include <dict.h>
#include <stdio.h>

// sets aside memory for the dictionary of sequence pointers based on the hash size
Dict *newDict(unsigned long long hashSize){
    Sequence** strings = malloc(sizeof(Sequence*)*hashSize);
    for(int i = 0; i < hashSize; ++i) strings[i] = NULL; // initialize
    
    Dict* dict = malloc(sizeof(Dict));
    dict->hashSize = hashSize;
    
    for(int i = 0; i < 256; i++){ // set the first sequences to 0 to 255
        Sequence *tempSequence = newSequence(i, hashSize);
        strings[tempSequence->hash] = tempSequence;
    }
    
    dict->strings = strings;
    dict->nextCode = 256;
    
    return dict;
}

// free all the sequences of the dictionary, and then the dictionary
void deleteDictDeep(Dict* dict) {
    unsigned long long i = 0;
    while(i < dict->hashSize){
        if(dict->strings[i] != NULL) deleteSequence(dict->strings[i]);
        i++;
    }
    free(dict->strings);
    free(dict);
}

// gets a character from the dictionary, to prevent needing to use more memory
Sequence *getCharFromDict(Dict* dict, unsigned char c){
    unsigned int hash = stringHash(&c, dict->hashSize, 1);
    return dict->strings[hash];
}

// searches the dictionary using the sequence's hash code
// also checks the dictionary's buckets (if they exist)
bool searchDict(Dict* dict, Sequence* key, unsigned int* code){
    if(key == NULL) return false;
    unsigned int hash = key->hash;
    if(dict->strings[hash] != NULL){
        if(identicalSequences(key, dict->strings[hash])){
            *code = dict->strings[hash]->code;
            return true;
        }
    }else{
        return false;
    }

    // For checking within the bucket
    Sequence *check;
    if(dict->strings[hash] != NULL) check = dict->strings[hash]->nextSequence;
    
    while(check != NULL){
        if(!identicalSequences(key, check)){
            check = check->nextSequence;
        }else{
            *code = check->code;
            return true;
        }
    }
    
    return false;
}

// inserts a sequence into the dictionary based on its hash code
void insertDict(Dict* dict, Sequence* key, unsigned int  code){
    key->code = code;
    
    if(dict->strings[key->hash] == NULL){
        dict->strings[key->hash] = key;
        return;
    }
    
    // if a collision occurred, add the sequence to the linked list of next sequences
    Sequence* tempSequence = dict->strings[key->hash];
    while(tempSequence->nextSequence != NULL){
        tempSequence = tempSequence->nextSequence;
    }
    
    tempSequence->nextSequence = key;
}
