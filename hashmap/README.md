HashMap Code
=============
In this folder he the basic boilerplate to run the HashMap locally is
given. You can pass additional features for compilation using the 
`FEATURES` flag in the Makefile. You can do this over the CLI
using the command `make FEATURES="-DNEW_HASH -DSEQUENTIAL"`.

At the moment, two different flags are defined:
- `NEW_HASH`: enables the code for the bonus hashing assignment
- `SEQUENTIAL`: runs the tests sequentially and not in parallel.
     This is needed to run GDB on your code. 

The following commands work:
- `make`: runs the tests
- `make compile`: compiles the code
- `make gdb`: starts the GDB debugger
- `make valgrind`: runs valgrind
