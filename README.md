# LZW-Encoding
Library for LZW Encoder and Decoder

Example main file included.

Make library files first, then link to example file using:
gcc -o lzw example.c -L./lzwLib -llzw


Usage of example main:
Arg 1: "encode" or "decode"
Arg 2: Number of bits     (between 8 and 24)
Arg 3: Max number of bits (between 8 and 24)
Arg 4: Path to input file
Arg 5: Path to output file

Encode example:
./lzw encode 17 24 infile.txt encoded.txt

Decode example:
./lzw decode 17 24 encoded.txt decoded.txt
