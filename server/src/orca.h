#ifndef __ORCA_H
#define __ORCA_H

/* Includes */
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <libnetconf.h>

/* Macro definitions */
#define ORCA_AGENTS_PATH "/usr/etc/netopeer/"
#define ORCA_AGENTS_FILE "agents.conf"
#define ORCA_AGGR_NAME	"ksi_aggregator"
#define ORCA_EXTD_NAME	"ksi_extender"

/* Global variables */
extern char	*orca_aggr_ns;
extern char	*orca_aggr_url;
extern char	*orca_extd_ns;
extern char	*orca_extd_url;

/* Function prototypes */
int orca_agents_parse();
int orca_agents_show();
void orca_cleanup();

#endif  /* __ORCA_H */

