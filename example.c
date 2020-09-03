#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// gcc -o test main.c -L./lzwLib -llzw

//
bool lzwEncode(unsigned int bits, unsigned int maxBits,
               int  (*readFunc )(void* context),
               void (*writeFunc)(unsigned char c, void* context),
               void* context);

bool lzwDecode(unsigned int bits, unsigned int maxBits,
               int  (*readFunc )(void* context),
               void (*writeFunc)(unsigned char c, void* context),
               void* context);

typedef struct _context {
    FILE *in;
    FILE *out;
} Context;

int read(void *context){
    Context* con = context;
    return fgetc(con->in);
}

void write(unsigned char c, void *context){
    Context* con = context;
    fputc(c, con->out);
}

int main (int argc, char* argv[]){
    
    Context* context = malloc(sizeof(Context));
    //context->in = stdin;
    //context->out = stdout;

    
    if(argc > 1){
        if(strcmp(argv[1], "encode") == 0){
            if(argc > 3) context->in = fopen(argv[4], "r");
            if(argc > 4) context->out = fopen(argv[5], "wb");
            lzwEncode(atoi(argv[2]), atoi(argv[3]), read,write,context);
            fclose(context->in);
            fclose(context->out);
        }else if(strcmp(argv[1], "decode") == 0){
            if(argc > 3) context->in = fopen(argv[4], "rb");
            if(argc > 4) context->out = fopen(argv[5], "w");
            lzwDecode(atoi(argv[2]), atoi(argv[3]), read, write, context);
            fclose(context->in);
            fclose(context->out);
        }
    }
    
    free(context);

    return 0;
}
