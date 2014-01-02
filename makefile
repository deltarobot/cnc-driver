objects = main driver
main_incl = driver
driver_incl = driver
driver_lib = bcm2835

cc=gcc -Isrc/include/ -Ilib/ -Wall -Wextra -pedantic -o

code = $(patsubst %,src/main/%.c,$1)
incl = $(patsubst %,src/include/%.h,$1)
lib = $(patsubst %,lib/%.o,$1)
targ = $(patsubst %,target/%.o,$1)
test = $(patsubst %,src/test/%_test.c,$1)
testexe = $(patsubst %,target/%_test,$1)

objects_no_main = $(filter-out main,$(objects))

define make_object =
$(call targ,$1): $(call code,$1) $(call incl,$($1_incl))
	$(cc) $(call targ,$1) -c $(call code,$1)
endef

define make_test =
$(call testexe,$1): $(call test,$1) $(call code,$1) $(call incl,$($1_incl)) $(call lib,$($1_lib) CuTest)
	$(cc) $(call testexe,$1) -Isrc/main/ $(call lib,$($1_lib) CuTest) $(call test,$1)
	target/$1_test; if [ $$$$? -ne 0 ]; then rm target/$1_test; return 1; fi
endef

all: $(call targ,$(objects)) $(call testexe,$(objects_no_main)) lib/bcm2835.o
	$(cc) target/driver lib/bcm2835.o $(call targ,$(objects))

$(foreach object,$(objects),$(eval $(call make_object,$(object))))

$(foreach object,$(objects_no_main),$(eval $(call make_test,$(object))))

lib/CuTest.o: lib/CuTest.c lib/CuTest.h
	$(cc) lib/CuTest.o -c lib/CuTest.c

lib/bcm2835.o: lib/bcm2835.c lib/bcm2835.h
	$(cc) lib/bcm2835.o -c lib/bcm2835.c

clean:
	rm -f target/*
