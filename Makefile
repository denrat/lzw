# Environment
SHELL = bash
PWD = $(shell pwd)

# Compilation
CC = clang
CFLAGS = -Wall -std=c99 -g
CFLAGS += -O3

ifdef DEBUG
CFLAGS += -DDEBUG
endif

OBJDIR = obj
OBJ =  $(OBJDIR)/lzw.o
OBJ += $(OBJDIR)/dictionary.o
OBJ += $(OBJDIR)/triple.o
OBJ += $(OBJDIR)/tools.o
OBJ += $(OBJDIR)/arguments.o

EXE = lzw

# Tests
ifndef TESTFILE
TESTFILE = f.txt
endif

.PHONY: all clean test cleantest

all: $(EXE)

#
# Binary
#

$(EXE): main.c $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

$(OBJDIR)/%.o: %.c %.h | $(OBJDIR)
	$(CC) $(CFLAGS) -o $@ -c $<

$(OBJDIR):
	@mkdir $@

#
# Tests
#

$(TESTFILE).lzw: $(EXE) $(TESTFILE)
	./lzw e $(TESTFILE)

$(TESTFILE).lzw.unlzw: $(EXE) $(TESTFILE).lzw
	./lzw d $(TESTFILE).lzw

test: cleantest $(EXE) $(TESTFILE) $(TESTFILE).lzw.unlzw
	@if [[ `md5 -q $(TESTFILE)` == `md5 -q $(TESTFILE).lzw.unlzw` ]]; then\
		echo "Checksums match!";\
	else\
		echo "Checksums don\'t match, compression failed." && exit 1;\
	fi

#
# Clean
#

clean: cleantest
	rm -f *.o $(EXE)
	rm -rf $(EXE).dSYM $(OBJDIR)

cleantest:
	rm -f *.lzw *.unlzw
