# CMSC216 Project 2 Makefile
AN = p2
CLASS = 216

# -Wno-comment: disable warnings for multi-line comments, present in some tests
# CFLAGS = -Wall -Wno-comment -Werror -g
CFLAGS = -Wall -Wno-comment -Werror -g -Wno-format-security 
CC     = gcc $(CFLAGS)
SHELL  = /bin/bash
CWD    = $(shell pwd | sed 's/.*\///g')

PROGRAMS = \
	thermo_main \
	test_thermo_update \
	puzzlebox   \
	hashmap_main \
	hashmap_demo_init \


all : $(PROGRAMS) 

# cleaning target to remove compiled programs/objects
clean :
	rm -f $(PROGRAMS) *.o

help :
	@echo 'Typical usage is:'
	@echo '  > make                          # build all programs'
	@echo '  > make clean                    # remove all compiled items'
	@echo '  > make zip                      # create a zip file for submission'
	@echo '  > make prob1                    # built targets associated with problem 1'
	@echo '  > make test                     # run all tests'
	@echo '  > make test-prob2               # run test for problem 2'
	@echo '  > make test-prob2 testnum=5     # run problem 2 test #5 only'


############################################################
# 'make zip' to create complete.zip for submission
ZIPNAME = $(AN)-complete.zip
zip : clean clean-tests
	rm -f $(ZIPNAME)
	cd .. && zip "$(CWD)/$(ZIPNAME)" -r "$(CWD)"
	@echo Zip created in $(ZIPNAME)
	@if (( $$(stat -c '%s' $(ZIPNAME)) > 10*(2**20) )); then echo "WARNING: $(ZIPNAME) seems REALLY big, check there are no abnormally large test files"; du -h $(ZIPNAME); fi
	@if (( $$(unzip -t $(ZIPNAME) | wc -l) > 256 )); then echo "WARNING: $(ZIPNAME) has 256 or more files in it which may cause submission problems"; fi

################################################################################
# thermometer problem
prob1 : thermo_main test_thermo_update

thermo_main : thermo_main.o thermo_update.o thermo_sim.o
	$(CC) -o $@ $^

thermo_main.o : thermo_main.c thermo.h
	$(CC) -c $<

thermo_sim.o : thermo_sim.c thermo.h
	$(CC) -c $<

thermo_update.o : thermo_update.c thermo.h
	$(CC) -c $<

test_thermo_update : test_thermo_update.c thermo_sim.o thermo_update.o
	$(CC) -o $@ $^

################################################################################
# debugging problem
prob2 : puzzlebox

puzzlebox.o : puzzlebox.c
	$(CC) -c $<

puzzlebox : puzzlebox.o
	$(CC) -o $@ $^


################################################################################
# hashmap problem
prob3: hashmap_main

hashmap_main : hashmap_main.o hashmap_funcs.o
	$(CC) -o $@ $^

hashmap_main.o : hashmap_main.c hashmap.h
	$(CC) -c $<

hashmap_funcs.o : hashmap_funcs.c hashmap.h
	$(CC) -c $<

hashmap_demo_init : hashmap_demo_init.c hashmap_funcs.o
	$(CC) -o $@ $^


# Testing Targets
test : test-prob1 test-prob2 test-prob3

test-setup:
	@chmod u+x testy

test-prob1 : prob1 test_thermo_update test-setup
	./testy test_thermo_update.org $(testnum)

test-prob2 : prob2 test-setup
	./puzzlebox input.txt

test-prob3 : hashmap_main test-setup
	./testy test_hashmap.org $(testnum) 

clean-tests :
	rm -rf test-results

