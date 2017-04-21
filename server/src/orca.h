#ifndef __ORCA_H
#define __ORCA_H

/* Includes */
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <libnetconf.h>

/* Macro definitions */
#define AGENTS_PATH "/usr/local/etc/netopeer/"
#define AGENTS_FILE "agents.conf"

/* Function prototypes */
int orca_agents_parse();

#endif  /* __ORCA_H */

