# CMSC216 Project 1 Makefile
AN = p1
CLASS = 216

# -Wno-comment: disable warnings for multi-line comments, present in some tests
CFLAGS = -Wall -Wno-comment -Werror -g -Wno-unused-variable
CC     = gcc $(CFLAGS)
SHELL  = /bin/bash
CWD    = $(shell pwd | sed 's/.*\///g')

PROGRAMS = \
	banlet_main  \
	banlet_demo \
	test_banlet_funcs \


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



# banlet problem
banlet_main : banlet_main.o banlet_funcs.o banlet_font_standard.o
	$(CC) -o $@ $^

banlet_main.o : banlet_main.c banlet.h
	$(CC) -c $<

banlet_funcs.o : banlet_funcs.c banlet.h
	$(CC) -c $<

banlet_font_standard.o : banlet_font_standard.c banlet.h
	$(CC) -c $<

banlet_demo : banlet_demo.c banlet_font_standard.o
	$(CC) -o $@ $^

test_banlet_funcs : test_banlet_funcs.c banlet_funcs.o banlet_font_standard.o
	$(CC) -o $@ $^


# problem targets
prob1 : banlet_funcs.o

prob2 : banlet_funcs.o

prob3 : banlet_main

# Testing Targets
test : test-prob1 test-prob2 test-prob3

test-setup:
	@chmod u+x testy

test-prob1 : test_banlet_funcs test-setup
	./testy test_banlet1.org $(testnum)

test-prob2 : test_banlet_funcs test-setup
	./testy test_banlet2.org $(testnum)

test-prob3 : banlet_main test-setup
	./testy test_banlet3.org $(testnum)

clean-tests :
	rm -rf test-results

