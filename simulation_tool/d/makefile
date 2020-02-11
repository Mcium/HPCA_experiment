CC=g++ 
CFLAGS=-I.
CXXFLAGS=-std=c++11
CFLAGS=-g
DEPS = param_read.hh core.hh
OBJ = main.o param_read.o core.o

%.o: %.c $(DEPS)
	$(CC) -c  -o $@ $< $(CFLAGS)

accelerator: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean
clean: 
	-rm accelerator $(OBJ)
