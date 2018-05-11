SHELL = bash

CC = clang
CFLAGS = -Wall -std=c99 -g

OBJDIR = obj

OBJ =  $(OBJDIR)/lzw.o
OBJ += $(OBJDIR)/dictionary.o
OBJ += $(OBJDIR)/triple.o
OBJ += $(OBJDIR)/tools.o

EXE = lzw

all: $(OBJDIR) $(EXE)

$(EXE): main.c $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

$(OBJDIR)/%.o: %.c %.h
	$(CC) $(CFLAGS) -o $@ -c $<

$(OBJDIR):
	@mkdir $@

testenc: $(EXE)
	./lzw e f.txt

testdec: $(EXE)
	./lzw d f.txt.lzw

testall: testenc testdec
	@if [[ `md5 -q f.txt` == `md5 -q f.txt.lzw.unlzw` ]];\
		then echo 'Checksums match!';\
		else echo 'Mismatching files.';\
		exit 1;\
	fi

clean:
	rm -f *.o $(EXE) *.lzw *.unlzw
	rm -rf $(EXE).dSYM $(OBJDIR)
