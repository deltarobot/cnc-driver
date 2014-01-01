cc=gcc -Isrc/include/ -Wall -Wextra -pedantic -o

code = $(patsubst %,src/main/%,$1)
incl = $(patsubst %,src/include/%,$1)
test = $(patsubst %,src/test/%,$1)
targ = $(patsubst %,target/%,$1)

all: $(call targ,main.o)
	$(cc) target/driver target/main.o

target/main.o: $(call code,main.c) $(call incl,driver.h)
	$(cc) target/main.o -c $(call code,main.c)

clean:
	rm -f target/*
