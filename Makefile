CC=gcc
CFLAGS= -g -Wall -Wextra -Werror -ansi -pedantic
proj1: *.c *.h
zip: *.c *.h
	zip proj1.zip *.c *.h
clean:
	rm -f proj1 tests/*.diff tests_diogo/community_tests/*.result proj1.zip