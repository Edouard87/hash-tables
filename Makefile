tester_binary = ./tmp/tester.out
tester_source = ./tester.c
compiler = gcc
compiler_args = -g3 -lcunit -v
valgrind_basic_opts = -v --tool=memcheck --leak-check=full --track-origins=yes --suppressions=suppressions.supp

clean:
	rm ./tmp/*.out

build_test:
	$(compiler) $(tester_source) ./hash_table.h ./hash_table.c -o $(tester_binary) $(compiler_args)

memcheck:
	make build_test && valgrind $(valgrind_basic_opts) $(tester_binary)

memcheck_debug:
	make build_test && valgrind $(valgrind_basic_opts) --vgdb-error=0 $(tester_binary)
