all:
	cd lzwLib && $(MAKE)
	gcc -o lzw example.c -L./lzwLib -llzw
	
clean:
	rm -f lzw
	cd lzwLib && rm -f liblzw.a
