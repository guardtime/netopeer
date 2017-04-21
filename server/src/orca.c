#include "orca.h"

int orca_agents_parse()
{
    FILE    *fp=NULL;
    char    *agent=NULL, *host=NULL, *port=NULL;
    int	    rv=0;

    fp = fopen(AGENTS_PATH"/"AGENTS_FILE, "r");
    if (fp == NULL)
    {
	nc_verb_verbose("fopen");
	rv = -1;
	goto cleanup;
    }

    while (!feof(fp))
    {
	rv = fscanf(fp, "%ms %ms %ms\n", &agent, &host, &port);
	if (rv > 0)
	{
	    printf("rv: %d agent: %s host: %s port: %s\n",
		    rv, agent, host, port);
	    free(agent);
	    free(host);
	    free(port);
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

