SHELL = bash
PWD = $(shell pwd)

CC = clang
CFLAGS = -Wall -std=c99 -g

OBJDIR = obj

OBJ =  $(OBJDIR)/lzw.o
OBJ += $(OBJDIR)/dictionary.o
OBJ += $(OBJDIR)/triple.o
OBJ += $(OBJDIR)/tools.o

EXE = lzw

ifndef TESTFILE
TESTFILE = f.txt
endif

all: $(EXE)

$(EXE): main.c $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

$(OBJDIR)/%.o: %.c %.h | $(OBJDIR)
	$(CC) $(CFLAGS) -o $@ -c $<

$(OBJDIR):
	@mkdir $@

$(TESTFILE).lzw: $(EXE) $(TESTFILE)
	./lzw e $(TESTFILE) > /dev/null

$(TESTFILE).lzw.unlzw: $(EXE) $(TESTFILE).lzw
	./lzw d $(TESTFILE).lzw > /dev/null

testenc: $(TESTFILE).lzw

testdec: $(TESTFILE).lzw.unlzw

testall: all $(TESTFILE) $(TESTFILE).lzw.unlzw | silent
ifeq ($(shell md5 -q $(TESTFILE)),$(shell md5 -q $(TESTFILE).lzw.unlzw))
	$(info Checksums match!)
else
	$(warning Checksums don’t match, lossless compression failed.)
endif

clean:
	rm -f *.o $(EXE) *.lzw *.unlzw
	rm -rf $(EXE).dSYM $(OBJDIR)

silent:
	@:

.PHONY: all clean testenc testdec testall silent
