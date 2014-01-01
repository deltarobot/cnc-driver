objects = main driver
main_incl = driver.h
driver_incl = driver.h

cc=gcc -Isrc/include/ -Wall -Wextra -pedantic -o

code = $(patsubst %,src/main/%,$1)
incl = $(patsubst %,src/include/%,$1)
test = $(patsubst %,src/test/%,$1)
testexe = $(patsubst %,target/%,$1_test)
targ = $(patsubst %,target/%,$1)

objects_no_main = $(filter-out main,$(objects))

define make_object =
$(call targ,$1.o): $(call code,$1.c) $(call incl,$($1_incl))
	$(cc) $(call targ,$1.o) -c $(call code,$1.c)
endef

define make_test =
$(call targ,$1_test): $(call test,$1_test.c) $(call code,$1.c) $(call incl,$($1_incl))
	$(cc) $(call targ,$1_test) -Isrc/main/ $(call test,$1_test.c)
	target/$1_test
endef

all: $(call targ,main.o) $(call testexe,$(objects_no_main))
	$(cc) target/driver target/main.o

$(foreach object,$(objects),$(eval $(call make_object,$(object))))

$(foreach object,$(objects_no_main),$(eval $(call make_test,$(object))))

clean:
	rm -f target/*
