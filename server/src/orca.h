#ifndef __ORCA_H
#define __ORCA_H

/* Includes */
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <libnetconf.h>
#include <curl/curl.h>

/* Macro definitions */
#define ORCA_AGENTS_PATH "/usr/etc/netopeer/"
#define ORCA_AGENTS_FILE "agents.conf"
#define ORCA_AGGR_NAME	"ksi_aggregator"
#define ORCA_EXTD_NAME	"ksi_extender"

#define ORCA_URI_REVISION   "/orca/revision"
/* NOTE: The actual config URI is /orca/{revision}/config. */
#define ORCA_URI_CONFIG	    "/config"

/* Global variables */
extern char	*orca_aggr_ns;
extern char	*orca_aggr_url;
extern char	*orca_extd_ns;
extern char	*orca_extd_url;
extern CURL	*curl;

/* Function prototypes */
int orca_init();
void orca_cleanup();
int orca_agents_parse();
int orca_agents_show();
int orca_revision_get(CURL *curl, const char *agent);
int orca_config_post(CURL *curl, const char *agent);
int orca_config_put(CURL *curl, const char *agent);

#endif  /* __ORCA_H */

