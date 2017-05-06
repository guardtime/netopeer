#ifndef __ORCA_H
#define __ORCA_H

#define _GNU_SOURCE

/* Includes */
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <libnetconf.h>
#include <curl/curl.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

/* Macro definitions */
#define ORCA_AGENTS_PATH "/usr/etc/netopeer/"
#define ORCA_AGENTS_FILE "agents.conf"
#define ORCA_AGGR_NAME	"ksi_aggregator"
#define ORCA_EXTD_NAME	"ksi_extender"

#define ORCA_URI_REVISION   "/orca/revision"
/* NOTE: The actual config URI is /orca/{revision}/config. */
#define ORCA_URI_CONFIG	    "/config"

#define ORCA_GET_CFG_START  "<get-config>"
#define ORCA_GET_CFG_END    "</get-config>"
#define ORCA_SET_CFG_START  "<set-config>"
#define ORCA_SET_CFG_END    "</set-config>"

/* Data types */
typedef struct {
    char    *name;
    char    *ns;
    char    *url;
} Orca_Agent;

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
char *orca_rpc_get(char *rpc, const char *start, const char *end);
int orca_get_agent(const char *rpc, const char *parent, Orca_Agent *agent);
char * orca_revision_get(CURL *curl, const char *agent);
char * orca_config_post(CURL *curl, const char *agent, const char *postdata);
char *orca_config_put(CURL *curl, const char *agent, const char *putdata);

#endif  /* __ORCA_H */

