SHELL = bash

CC = clang
CFLAGS = -Wall -std=c99 -g
LZWFUNC = lzw.o
STACK = stack.o

EXE = lzw

TESTCASE = f.txt

all: $(EXE)

$(EXE): main.c $(LZWFUNC) $(STACK)
	$(CC) $(CFLAGS) -o $@ $^

$(LZWFUNC): lzw.c lzw.h
	$(CC) $(CFLAGS) -c $<

$(STACK): stack.c stack.h
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o $(EXE) *.lzw *.unlzw
	rm -rf $(EXE).dSYM

test: $(EXE)
	@./$(EXE) e $(TESTCASE)
	@./$(EXE) d $(TESTCASE).lzw
	@test `shasum $(TESTCASE)` -eq `shasum $(TESTCASE).lzw.unlzw` && echo Success! || echo Failed.
