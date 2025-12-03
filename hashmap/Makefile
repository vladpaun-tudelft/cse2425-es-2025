# Usage:
# Run 'make test' to execute the test program.
# Run 'make valgrind' to run the test program in Valgrind.
# Run 'make clean' to remove compiled files.

CC = gcc
CFLAGS = -g -Wall -Werror -Wextra -Wno-unused-parameter -Wno-unused-variable -pedantic
LDFLAGS += -lpthread -lrt
OBJS = test.c gest.c
TARGET = test
FEATURES = 

.PHONY: test valgrind submit clean

default: test

compile : $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) $(CFLAGS) $(FEATURES) -o $(TARGET)

test: compile
	@echo -e "\n\\033[1;4m[Output]\\033[0m"
	@./test | egrep  "failed|passed"
	@echo -e "\n\\033[31;1;4m[Errors]\\033[0m "
	@./test |& egrep -i -B1 "test\.c|fault|segmentation" | sed -- "s/--//g" 
	@exit $(.SHELLSTATUS)


valgrind: compile
	valgrind --tool=memcheck ./test

clean:
	-rm -f *.o
	-rm -f $(TARGET)
gdb:
	gdb --tui ./test
