SHELL = bash
PWD = $(shell pwd)

CC = clang
CFLAGS = -Wall -std=c99 -g

OBJDIR = obj
OBJ =  $(OBJDIR)/lzw.o
OBJ += $(OBJDIR)/dictionary.o
OBJ += $(OBJDIR)/triple.o
OBJ += $(OBJDIR)/tools.o
OBJ += $(OBJDIR)/arguments.o

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

test: $(EXE) $(TESTFILE) $(TESTFILE).lzw.unlzw
	@if [[ `md5 -q $(TESTFILE)` == `md5 -q $(TESTFILE).lzw.unlzw` ]]; then\
		echo "Checksums match!";\
	else\
		echo "Checksums don\'t match, compression failed." && exit 1;\
	fi

clean: cleantest
	rm -f *.o $(EXE)
	rm -rf $(EXE).dSYM $(OBJDIR)

cleantest:
	rm -f *.lzw *.unlzw

.PHONY: all clean test cleantest
