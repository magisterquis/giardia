# giardia
Backdoor code for C binaries.

Work in progress.

Please see [example.c](example/example.c) for an example.

It should be sufficient to include giardia.c and giardia.h in the source of the
binary to backdoor, add one line to fork giardia off, probably somewhere soon
after main() (see the example), and rebuild the binary.

Connecting to the backdoor is done with netcat.  The first line sent should be
the password given in the call to giardia.
