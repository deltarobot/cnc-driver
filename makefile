objects = main
main_incl = driver.h

cc=gcc -Isrc/include/ -Wall -Wextra -pedantic -o

code = $(patsubst %,src/main/%,$1)
incl = $(patsubst %,src/include/%,$1)
test = $(patsubst %,src/test/%,$1)
targ = $(patsubst %,target/%,$1)

define make_object =
$(call targ,$1.o): $(call code,$1.c) $(call incl,$($1_incl))
	$(cc) $(call targ,$1.o) -c $(call code,$1.c)
endef

all: $(call targ,main.o)
	$(cc) target/driver target/main.o

$(foreach object,$(objects),$(eval $(call make_object,$(object))))

clean:
	rm -f target/*
