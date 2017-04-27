/* Includes */
#include "orca.h"

/* Local data structures */
struct Orca_MemBlock {
  char *buffer;
  size_t size;
};

/* Gloval variables */

/* Module code */
/**********************************************************************/
static size_t orca_write_callback(void *data, size_t size, size_t nmemb,
				  void *userp)
{
  size_t realsize = size * nmemb;
  struct Orca_MemBlock *mem = (struct Orca_MemBlock *)userp;

  mem->buffer = realloc(mem->buffer, mem->size + realsize + 1);
  if(mem->buffer == NULL) {
    nc_verb_error("realloc");
    exit(EXIT_FAILURE);
  }

  memcpy(&(mem->buffer[mem->size]), data, realsize);
  mem->size += realsize;
  mem->buffer[mem->size] = 0;

  return realsize;
}

/**********************************************************************/
int orca_agents_parse()
{
    FILE    *fp=NULL;
    char    *agent=NULL, *host=NULL;
    int	    rv=0;

    fp = fopen(ORCA_AGENTS_PATH"/"ORCA_AGENTS_FILE, "r");
    if (fp == NULL)
    {
	nc_verb_error("fopen");
	rv = -1;
	goto cleanup;
    }

    while (!feof(fp))
    {
	rv = fscanf(fp, "%ms %ms\n", &agent, &host);
	if (rv > 0)
	{
	    nc_verb_verbose("rv: %d agent: %s host: %s\n",
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
		nc_verb_error("Unsupported agent: %s", agent);
		free(agent);
		free(host);
	    }
	    rv = 0;
	}
	else if (errno != 0)
	{
	    nc_verb_error("scanf");
	    rv = -1;
	}
	else  /* EOF */
	{
	    nc_verb_warning("No matching pattern found");
	    rv = -1;
	}
    }

cleanup:
    if ((fclose(fp)) != 0)
    {
	nc_verb_error("fclose");
	rv = -1;
	goto cleanup;
    }

    return rv;
}

/**********************************************************************/
int orca_agents_show()
{
    int rv=0;

    nc_verb_verbose("ksi_aggregator ns: %s\turl: %s",
	    orca_aggr_ns, orca_aggr_url);
    nc_verb_verbose("ksi_extender ns: %s\turl: %s",
	    orca_extd_ns, orca_extd_url);

    return rv;
}

/**********************************************************************/
void orca_cleanup()
{
    free(orca_aggr_ns);
    free(orca_aggr_url);
    free(orca_extd_ns);
    free(orca_extd_url);

    if (curl)
	curl_easy_cleanup(curl);
    curl_global_cleanup();
}

/**********************************************************************/
int orca_init()
{
    int	rv=0;

    if (orca_agents_parse() != 0) {
	nc_verb_error("Could not parse agents.conf");
	rv = -1;
	goto cleanup;
    }
    orca_agents_show();
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if (curl == NULL) {
	nc_verb_error("curl_easy_init");
	rv = -1;
	goto cleanup;
    }
    orca_revision_get(curl, orca_aggr_url);

cleanup:
    return rv;
}

/**********************************************************************/
int orca_revision_get(CURL *curl, const char *agent)
{
    char    *url=NULL;
    size_t  url_size=0;
    int	    rv=0;
    struct Orca_MemBlock resp;

    /* size_t orca_write_callback(void *data, size_t size, size_t nmemb,
				  void *userp) */
    if ((curl == NULL) || (agent == NULL)) {
	nc_verb_error("%s: Invalid argument", __func__);
	rv = -1;
	goto cleanup;
    }

    resp.buffer = malloc(1);
    resp.size = 0;

    url_size = strlen(agent) + strlen(ORCA_URI_REVISION) + 1;
    url = malloc(url_size);
    nc_verb_verbose("url_size: %lu url@ %p\n", url_size, url);
    if (url == NULL) {
	nc_verb_error("%s: malloc", __func__);
	exit(EXIT_FAILURE);
    }
    memset(url, 0, url_size);
    sprintf(url, "%s%s", agent, ORCA_URI_REVISION);
    nc_verb_verbose("%s: url: %s length: %d", __func__, url, strlen(url));

    rv = curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
    rv |= curl_easy_setopt(curl, CURLOPT_URL, url);
    rv |= curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, orca_write_callback);
    rv |= curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&resp);

    if (rv != CURLE_OK) {
	nc_verb_error("%s:%d:%s: curl_easy_setopt",
		__FILE__, __LINE__, __func__);
	goto cleanup;
    }

    rv = curl_easy_perform(curl);
    if (rv != CURLE_OK) {
	nc_verb_error("%s:%d:%s: curl_easy_perform",
		__FILE__, __LINE__, __func__);
	goto cleanup;
    }
    nc_verb_verbose("Agent reply: %s", resp.buffer);

cleanup:
    free(url);
    return rv;
}

/**********************************************************************/
int orca_config_post(CURL *curl, const char *agent)
{
    char    *url=NULL;
    char    data[BUFSIZ];
    int	    rv=0;

    if ((curl == NULL) || (agent == NULL)) {
	nc_verb_error("%s: Invalid argument", __func__);
	rv = -1;
	goto cleanup;
    }

    memset(data, 0, BUFSIZ);
    sprintf(data, "%s%s", url, ORCA_URI_REVISION);
    nc_verb_verbose("%s: data: %s", __func__, data);

    rv = curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);
    rv |= curl_easy_setopt(curl, CURLOPT_URL, url);
    if (rv != CURLE_OK) {
	nc_verb_error("%s:%d:%s: curl_east_setopt",
		__FILE__, __LINE__, __func__);
	goto cleanup;
    }
    rv = curl_easy_perform(curl);

cleanup:
    return rv;
}

/**********************************************************************/
int orca_config_put(CURL *curl, const char *agent)
{
    char    *url=NULL;
    char    data[BUFSIZ];
    int	    rv=0;

    if ((curl == NULL) || (agent == NULL)) {
	nc_verb_error("%s: Invalid argument", __func__);
	rv = -1;
	goto cleanup;
    }

    memset(data, 0, BUFSIZ);
    //sprintf(data, "%s/orca/%s%s", url, orca_extd_rev, ORCA_URI_CONFIG);
    nc_verb_verbose("%s: data: %s", __func__, data);

    rv = curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);
    rv |= curl_easy_setopt(curl, CURLOPT_URL, url);
    if (rv != CURLE_OK) {
	nc_verb_error("%s:%d:%s: curl_east_setopt",
		__FILE__, __LINE__, __func__);
	goto cleanup;
    }
    rv = curl_easy_perform(curl);

cleanup:
    return rv;
}

/**********************************************************************/

