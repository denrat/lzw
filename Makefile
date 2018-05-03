SHELL = bash

CC = clang
CFLAGS = -Wall -std=c99 -g

OBJDIR = obj

OBJ =  $(OBJDIR)/lzw.o
OBJ += $(OBJDIR)/dictionary.o
OBJ += $(OBJDIR)/triple.o
OBJ += $(OBJDIR)/stack.o # to be removed TODO
OBJ += $(OBJDIR)/tools.o

EXE = lzw

all: $(OBJDIR) $(EXE)

$(EXE): main.c $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

$(OBJDIR)/%.o: %.c %.h
	$(CC) $(CFLAGS) -o $@ -c $<

$(OBJDIR):
	@mkdir $@

clean:
	rm -f *.o $(EXE) *.lzw *.unlzw
	rm -rf $(EXE).dSYM $(OBJDIR)
