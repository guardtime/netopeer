#include "orca.h"

int orca_agents_parse()
{
    FILE    *fp=NULL;
    char    *agent=NULL, *host=NULL;
    int	    rv=0;

    fp = fopen(ORCA_AGENTS_PATH"/"ORCA_AGENTS_FILE, "r");
    if (fp == NULL)
    {
	nc_verb_verbose("fopen");
	rv = -1;
	goto cleanup;
    }

    while (!feof(fp))
    {
	rv = fscanf(fp, "%ms %ms\n", &agent, &host);
	if (rv > 0)
	{
	    printf("rv: %d agent: %s host: %s\n",
		    rv, agent, host);
	    if (strstr(agent, ORCA_AGGR_NAME))
	    {
		orca_aggr_ns = agent;
		orca_aggr_url = host;
	    }
	    else if (strstr(agent, ORCA_EXTD_NAME))
	    {
		orca_extd_ns = agent;
		orca_extd_url = host;
	    }
	    else
	    {
		nc_verb_verbose("Unsupported agent: %s", agent);
		free(agent);
		free(host);
	    }
	}
	else if (errno != 0)
	{
	    nc_verb_verbose("scanf");
	    rv = -1;
	}
	else  /* EOF */
	{
	    fprintf(stderr, "No matching pattern found\n");
	    rv = -1;
	}
    }

    if ((rv = fclose(fp)) != 0)
    {
	perror("fclose");
	rv = -1;
	goto cleanup;
    }

cleanup:
    return rv;
}


int orca_agents_show()
{
    int rv=0;

    nc_verb_verbose("ksi_aggregator ns: %s\turl: %s",
	    orca_aggr_ns, orca_aggr_url);
    nc_verb_verbose("ksi_extender ns: %s\turl: %s",
	    orca_extd_ns, orca_extd_url);

    return rv;
}


void orca_cleanup()
{
    free(orca_extd_ns);
    free(orca_extd_url);
    free(orca_extd_ns);
    free(orca_extd_url);
}

