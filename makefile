objects = main driver
main_incl = driver.h
driver_incl = driver.h
driver_lib = bcm2835.o

cc=gcc -Isrc/include/ -Ilib/ -Wall -Wextra -pedantic -o

code = $(patsubst %,src/main/%,$1)
incl = $(patsubst %,src/include/%,$1)
lib = $(patsubst %,lib/%,$1)
targ = $(patsubst %,target/%,$1)
test = $(patsubst %,src/test/%,$1)
testexe = $(patsubst %,target/%,$1_test)

objects_no_main = $(filter-out main,$(objects))

define make_object =
$(call targ,$1.o): $(call code,$1.c) $(call incl,$($1_incl))
	$(cc) $(call targ,$1.o) -c $(call code,$1.c)
endef

define make_test =
$(call targ,$1_test): $(call test,$1_test.c) $(call code,$1.c) $(call incl,$($1_incl)) $(call lib,$($1_lib)) lib/CuTest.o
	$(cc) $(call targ,$1_test) -Isrc/main/ $(call lib,$($1_lib)) lib/CuTest.o $(call test,$1_test.c)
	target/$1_test; if [ $$$$? -ne 0 ]; then rm target/$1_test; return 1; fi
endef

all: $(call targ,main.o driver.o) $(call testexe,$(objects_no_main)) lib/bcm2835.o
	$(cc) target/driver lib/bcm2835.o $(call targ,main.o driver.o)

$(foreach object,$(objects),$(eval $(call make_object,$(object))))

$(foreach object,$(objects_no_main),$(eval $(call make_test,$(object))))

lib/CuTest.o: lib/CuTest.c lib/CuTest.h
	$(cc) lib/CuTest.o -c lib/CuTest.c

lib/bcm2835.o: lib/bcm2835.c lib/bcm2835.h
	$(cc) lib/bcm2835.o -c lib/bcm2835.c

clean:
	rm -f target/*
