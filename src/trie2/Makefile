# CC = gcc


CC=gcc
CFLAGS=-std=c99 -I.
DBG_CFLAGS=-std=c99 -I. -v
DEPS = trie.h
OBJ = trie.o testTrie.o 

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

testTrieO: $(OBJ)
	gcc -g -o $@ $^ $(CFLAGS)

clean:
	rm -f $(OBJ)

debug: $(OBJ)
	gcc -g -o $@ $^ $(DBG_CFLAGS)

