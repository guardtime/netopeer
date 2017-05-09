/* Includes */
#include "orca.h"

/* Local data structures */
struct Orca_MemBlock {
  char *buffer;
  size_t size;
};

/* Gloval variables */
char *orca_rev = NULL;

/* Module code */
/**********************************************************************/
#ifdef UNUSED  /* Suppress compiler warnings about unused functions */
static xmlChar *get_ns(xmlNode *node)
{
    xmlChar *rv=NULL;

    if (node == NULL)
    {
	nc_verb_error("%s:%d:%s: Invalid argument",
		__FILE__, __LINE__, __func__);
	goto cleanup;
    }

    if (node->ns == NULL)
    {
	nc_verb_verbose("%s:%d:%s: %s has no namespace",
		__FILE__, __LINE__, __func__, node->name);
	rv = NULL;
	goto cleanup;
    }

    rv = (xmlChar *)node->ns->href;
    nc_verb_verbose("ns(%s): %s", node->name, rv);

cleanup:
    return rv;
}
#endif  /* UNUSED */

/**********************************************************************/
#ifdef UNUSED  /* Suppress compiler warnings about unused functions */
static xmlChar *get_attr(xmlNode *node, const char *attr)
{
    xmlChar *buf=NULL;

    nc_verb_verbose("%s:%d:%s", __FILE__, __LINE__, __func__);
    if ((node == NULL) || (attr == NULL)) {
	nc_verb_error("%s:%d:%s: Invalid argument",
		__FILE__, __LINE__, __func__);
	return NULL;
    }

    buf = xmlGetProp(node, (const xmlChar *)attr);
    nc_verb_verbose("%s.%s: %s", node->name, attr, buf);

    return buf;
}
#endif  /* UNUSED */

/**********************************************************************/
//#ifdef UNUSED  /* Suppress compiler warnings about unused functions */
static xmlNode *get_node(xmlDoc *doc, const char *elem)
{
    xmlNode *cur=NULL;

    //nc_verb_verbose("%s:%d:%s", __FILE__, __LINE__, __func__);
    if ((doc == NULL) || (elem == NULL)) {
	nc_verb_error("%s:%d:%s: Invalid argument",
		__FILE__, __LINE__, __func__);
	goto cleanup;
    }

    cur = xmlDocGetRootElement(doc);

    if (cur == NULL) {
	nc_verb_verbose("Null root element");
	goto cleanup;
    }

    if ((!xmlStrcmp(cur->name, (const xmlChar *)elem))) {
	goto cleanup;
    }

    cur = cur->xmlChildrenNode;
    while (cur != NULL) {
	nc_verb_verbose("%s: name: %s", __func__, cur->name);
	if ((!xmlStrcmp(cur->name, (const xmlChar *)elem))) {
	    goto cleanup;
	}
	cur = cur->next;
    }

cleanup:
    return cur;
}
//#endif  /* UNUSED */

/**********************************************************************/
//#ifdef UNUSED  /* Suppress compiler warnings about unused functions */
static char *get_reply_element(const char *data, const char *elem)
{
    char    *rv=NULL;
    xmlDoc  *doc=NULL;
    //xmlNode *node=NULL;

    if ((data == NULL) || (elem == NULL)) {
	goto cleanup;
    }

    LIBXML_TEST_VERSION

    doc = xmlReadMemory(data, strlen(data), "data.xml", NULL, 0);
    if (doc == NULL) {
        nc_verb_error("%s:%d:%s: xmlReadMemory failed",
		__FILE__, __LINE__, __func__);
	goto cleanup;
    }

    //node = get_node(doc, elem);
    rv = (char *)xmlNodeGetContent(get_node(doc, elem));
    nc_verb_error("%s: %s: %s", __func__, elem, rv);

cleanup:
    xmlFreeDoc(doc);
    xmlCleanupParser();
    return rv;
}
//#endif  /* UNUSED */
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
char *orca_rpc_get(char *rpc, const char *start, const char *end)
{
    char *rv=NULL, *tmp=NULL;

    if ((rpc == NULL) || (start == NULL) || (end == NULL)) {
        nc_verb_error("%s:%d:%s: Invalid argument",
		__FILE__, __LINE__, __func__);
	goto cleanup;
    }

    rv = strstr(rpc, start);
    tmp = strstr(rpc, end);

    if ((rv != NULL) && (tmp != NULL)) {
	*(tmp + strlen(end)) = '\0';
	nc_verb_verbose("RPC:\n%s", rv);
    } else {
	rv = NULL;
	goto cleanup;
    }

cleanup:
    return rv;
}

/**********************************************************************/
int orca_get_agent(const char *rpc, const char *parent, Orca_Agent *agent)
{
    char	*data=NULL, *tmp=NULL;
    xmlDocPtr	doc=NULL;
    xmlNodePtr	cur=NULL;
    int		rv=0;

    if ((rpc == NULL) || (parent == NULL) || (agent == NULL)) {
	nc_verb_error("%s:%d:%s: Invalid argument",
		__FILE__, __LINE__, __func__);
	rv = -1;
	goto cleanup;
    }
    nc_verb_verbose("%s:%d:%s: rpc: %s\nparent: %s",
	    __FILE__, __LINE__, __func__, rpc, parent);

    /*
     * If the parent element is contained in "<>" remove them,
     * otherwise XML name comparisons will fail.
     */
    tmp = (char *)parent;
    if (parent[0] == '<') {
	tmp++;
    }

    data = strdup(tmp);
    if (data == NULL) {
	/* Out of memory */
	nc_verb_error("strdup failed");
	exit(EXIT_FAILURE);
    }

    if (data[strlen(data)-1] == '>') {
	data[strlen(data)-1] = '\0';
    }
    nc_verb_verbose("%s:%d:%s: *** modified element:\n%s",
		__FILE__, __LINE__, __func__, data);

    /*
     * TODO: Consider using ncxml_rpc_get_op_content() on the caller's
     * side so that an xmlNodePtr can be passed as input instead of
     * parsing the XML rpc.
     *
     * Parse the XML rpc in the buffer.
     * This macro initializes the library.
     */
    LIBXML_TEST_VERSION

    doc = xmlReadMemory(rpc, strlen(rpc), "rpc.xml", NULL, 0);
    if (doc == NULL) {
        nc_verb_error("%s:%d:%s: xmlReadMemory failed",
		__FILE__, __LINE__, __func__);
	rv = -1;
	goto cleanup;
    }

    cur = xmlDocGetRootElement(doc);

    if (cur == NULL) {
	nc_verb_error("%s:%d:%s: Null root element",
		__FILE__, __LINE__, __func__);
	rv = -1;
	goto cleanup;
    }

    nc_verb_verbose("%s: Root name: %s\n", __func__, cur->name);
    cur = cur->xmlChildrenNode;

    /*
     * The agent element (aggregator or extender) must be inside a
     * filter element.
     */
    while (cur != NULL) {
	nc_verb_verbose("%s: AAA name: %s\n", __func__, cur->name);
	if (!xmlStrcmp(cur->name, (const xmlChar *)data)) {
	    nc_verb_verbose("%s: cur: %s", __func__, cur->name);
	    nc_verb_verbose("%s: *** Parent found ***", __func__);
	    cur = cur->xmlChildrenNode;
	    break;
	}
	cur = cur->next;
    }

    if (cur == NULL) {
	nc_verb_error("%s: Parent element %s not found", __func__, data);
	rv = -1;
	goto cleanup;
    }

    while (cur != NULL) {
	nc_verb_verbose("%s: BBB cur: %s", __func__, cur->name);
	if ((!xmlStrcmp(cur->name, (const xmlChar *)"aggregator")) ||
	    (!xmlStrcmp(cur->name, (const xmlChar *)"extender"))) {
	    nc_verb_verbose("%s: cur: %s", __func__, cur->name);
	    nc_verb_verbose("%s: *** Target found ***", __func__);
	    break;
	}
	cur = cur->next;
    }

    if ((cur) && (cur->ns)) {
	if ((strstr((char *)cur->ns->href, "guardtime")) == NULL) {
	    nc_verb_error("%s: Wrong namespace: %s",
		    __func__, cur->ns->href);
	    rv = -1;
	    goto cleanup;
	} else {
	    nc_verb_verbose("%s: target: %s ns: %s",
		    __func__, cur->name, cur->ns->href);}
    } else {
	nc_verb_error("%s: Agent not found", __func__);
	rv = -1;
	goto cleanup;
    }

    /* At this point we know the target name and namespace. */
    if (!strcmp(orca_aggr_ns, (char *)cur->ns->href)) {
	agent->name = ORCA_AGGR_NAME;
	agent->ns = orca_aggr_ns;
	agent->url = orca_aggr_url;
	rv = 0;
    } else if (!strcmp(orca_extd_ns, (char *)cur->ns->href)) {
	agent->name = ORCA_EXTD_NAME;
	agent->ns = orca_extd_ns;
	agent->url = orca_extd_url;
	rv = 0;
    } else {
	nc_verb_error("%s:%d:%s: Unknown error", __FILE__, __LINE__, __func__);
    }

    nc_verb_verbose("%s:%d:%s: agent: %s",
	    __FILE__, __LINE__, __func__, agent->name);

cleanup:
    xmlFreeDoc(doc);
    xmlCleanupParser();
    free(data);
    return rv;
}

/**********************************************************************/
int orca_agents_parse()
{
    FILE    *fp=NULL;
    char    *agent=NULL, *host=NULL;
    int	    rv=0;

    fp = fopen(ORCA_AGENTS_PATH"/"ORCA_AGENTS_FILE, "r");
    if (fp == NULL) {
	nc_verb_error("fopen");
	rv = -1;
	goto cleanup;
    }

    while (!feof(fp)) {
	rv = fscanf(fp, "%ms %ms\n", &agent, &host);
	if (rv > 0) {
	    nc_verb_verbose("rv: %d agent: %s host: %s\n",
		    rv, agent, host);
	    if (strstr(agent, ORCA_AGGR_NAME)) {
		orca_aggr_ns = agent;
		orca_aggr_url = host;
	    } else if (strstr(agent, ORCA_EXTD_NAME)) {
		orca_extd_ns = agent;
		orca_extd_url = host;
	    } else {
		nc_verb_error("Unsupported agent: %s", agent);
		free(agent);
		free(host);
	    }
	    rv = 0;
	} else if (errno != 0) {
	    nc_verb_error("scanf");
	    rv = -1;
	} else {  /* EOF */
	    nc_verb_warning("No matching pattern found");
	    rv = -1;
	}
    }

cleanup:
    if ((fclose(fp)) != 0) {
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

    nc_verb_verbose("%s: ksi_aggregator ns: %s\turl: %s",
	    __func__, orca_aggr_ns, orca_aggr_url);
    nc_verb_verbose("%s: ksi_extender ns: %s\turl: %s",
	    __func__, orca_extd_ns, orca_extd_url);

    return rv;
}

/**********************************************************************/
void orca_cleanup()
{
    free(orca_aggr_ns);
    free(orca_aggr_url);
    free(orca_extd_ns);
    free(orca_extd_url);
    free(orca_rev);

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
#if 0
    curl = curl_easy_init();
    if (curl == NULL) {
	nc_verb_error("curl_easy_init");
	rv = -1;
	goto cleanup;
    }
#endif  /* 0 */

cleanup:
    return rv;
}

/**********************************************************************/
char * orca_revision_get(CURL *curl, const char *agent, long *status)
{
    char		    *url=NULL;
    xmlDocPtr		    doc=NULL;
    long		    response_code=0;
    size_t		    url_size=0;
    int			    rv=0;
    struct Orca_MemBlock    resp={.buffer=NULL, .size=0};

    if ((curl == NULL) || (agent == NULL)) {
	nc_verb_error("%s: Invalid argument", __func__);
	rv = -1;
	goto cleanup;
    }

    *status = 0;
    resp.size = 0;
    resp.buffer = malloc(1);
    if (resp.buffer == NULL) {
	nc_verb_error("%s:%d:%s: malloc", __FILE__, __LINE__, __func__);
	exit(EXIT_FAILURE);
    }

    url_size = strlen(agent) + strlen(ORCA_URI_REVISION) + 1;
    url = malloc(url_size);
    //nc_verb_verbose("url_size: %lu url@ %p\n", url_size, url);
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
	nc_verb_error("%s:%d:%s: curl_easy_setopt: %d",
		__FILE__, __LINE__, __func__, rv);
	*status = (long)rv;
	goto cleanup;
    }

    rv = curl_easy_perform(curl);
    if (rv != CURLE_OK) {
	nc_verb_error("%s:%d:%s: curl_easy_perform: %d",
		__FILE__, __LINE__, __func__, rv);
	*status = (long)rv;
	goto cleanup;
    }

    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
    nc_verb_verbose("Response: %ld", response_code);
    nc_verb_verbose("Agent reply: %s", resp.buffer);
    *status = response_code;

    /*
     * Parse the XML document in the buffer.
     * This macro initializes the library.
     */
    LIBXML_TEST_VERSION

    doc = xmlReadMemory(resp.buffer, strlen(resp.buffer),
			"reply.xml", NULL, 0);
    if (doc == NULL) {
        nc_verb_error("xmlReadMemory: Failed to parse agent reply");
	goto cleanup;
    }

    if (orca_rev != NULL) {
	/*
	 * The size of the new revision shouldn't be different from the old
	 * one but free and reallocate just in case.
	 */
	free(orca_rev);
    }
    asprintf(&orca_rev, "%s", xmlNodeGetContent(xmlDocGetRootElement(doc)));
    nc_verb_verbose("Agent Revision: %s", orca_rev);

cleanup:
    xmlFreeDoc(doc);
    xmlCleanupParser();
    free(resp.buffer);
    free(url);
    return orca_rev;
}

/**********************************************************************/
char * orca_config_post(CURL *curl, const char *agent, const char *postdata,
			long *status)
{
    char    *url=NULL;
    char    *orca_resp=NULL;
    long    response_code=0;
    int	    rv=0;
    struct Orca_MemBlock resp={.buffer=NULL, .size=0};

    if ((curl == NULL) || (agent == NULL) || (postdata == NULL)) {
	nc_verb_error("%s: Invalid argument", __func__);
	rv = -1;
	goto cleanup;
    }
    nc_verb_verbose("%s: postdata: %s", __func__, postdata);

    if ((orca_rev = orca_revision_get(curl, agent, status)) == NULL) {
	nc_verb_error("%s:%d:%s: Revision not found",
		__FILE__, __LINE__, __func__);
	rv = -1;
	*status = -1;
	orca_resp = strdup("ERROR: Orca revision not found");
	goto cleanup;
    }

    *status = 0;
    resp.size = 0;
    resp.buffer = malloc(1);
    if (resp.buffer == NULL) {
	nc_verb_error("%s:%d:%s: malloc", __FILE__, __LINE__, __func__);
	exit(EXIT_FAILURE);
    }

    rv = asprintf(&url, "%s/orca/%s%s",
		    agent, orca_rev, ORCA_URI_CONFIG);

    if (rv == -1) {
	nc_verb_error("%s:%d:%s: asprintf", __FILE__, __LINE__, __func__);
	exit(EXIT_FAILURE);
    }
    rv = 0;
    //url_size = strlen(url);
    //nc_verb_verbose("url_size: %lu url@ %p\n", url_size, url);
    nc_verb_verbose("%s: url: %s length: %d\npostdata: %s length: %d",
	    __func__, url, strlen(url), postdata, strlen(postdata));

    rv |= curl_easy_setopt(curl, CURLOPT_POST, 1L);
    rv |= curl_easy_setopt(curl, CURLOPT_URL, url);
    rv |= curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, orca_write_callback);
    rv |= curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&resp);
    rv |= curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postdata);
    rv |= curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long)strlen(postdata));

    if (rv != CURLE_OK) {
	nc_verb_error("%s:%d:%s: curl_easy_setopt: %d",
		__FILE__, __LINE__, __func__, rv);
	orca_resp = strdup("ERROR: curl_easy_setopt");
	goto cleanup;
    }

    rv = curl_easy_perform(curl);
    if (rv != CURLE_OK) {
	nc_verb_error("%s:%d:%s: curl_easy_perform: %d",
		__FILE__, __LINE__, __func__, rv);
	orca_resp = strdup("ERROR: curl_easy_perform");
	goto cleanup;
    }

    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
    nc_verb_verbose("Response: %ld", response_code);
    *status = response_code;

    /*
     * If there is a message element an application-level error has
     * occured and no data should be present in the agent reply.
     */
    if (strstr(resp.buffer, "message") != NULL) {
	orca_resp = get_reply_element(resp.buffer, "message");
	nc_verb_error("ERROR: %s", orca_resp);
	*status = -1;
	goto cleanup;
    }

    orca_resp = strdup(resp.buffer);
    nc_verb_verbose("Agent response: %s", orca_resp);

cleanup:
    free(resp.buffer);
    free(url);
    return orca_resp;
}

/**********************************************************************/
char *orca_config_put(CURL *curl, const char *agent, const char *putdata,
		      long *status)
{
    char		    *url=NULL, *orca_resp=NULL;
    long		    response_code;
    int			    rv=0;
    struct Orca_MemBlock    resp={.buffer=NULL, .size=0};

    if ((curl == NULL) || (agent == NULL) || (putdata == NULL)) {
	nc_verb_error("%s: Invalid argument", __func__);
	rv = -1;
	goto cleanup;
    }
    nc_verb_verbose("%s: putdata: %s", __func__, putdata);

    if ((orca_rev = orca_revision_get(curl, agent, status)) == NULL) {
	nc_verb_error("%s:%d:%s: %ld Revision not found",
		__FILE__, __LINE__, __func__, *status);
	rv = -1;
	*status = -1;
	orca_resp = strdup("ERROR: Orca revision not found");
	goto cleanup;
    }

    *status = 0;
    resp.size = 0;
    resp.buffer = malloc(1);
    if (resp.buffer == NULL) {
	nc_verb_error("%s:%d:%s: malloc", __FILE__, __LINE__, __func__);
	exit(EXIT_FAILURE);
    }

    rv = asprintf(&url, "%s/orca/%s%s",
		    agent, orca_rev, ORCA_URI_CONFIG);

    if (rv == -1) {
	nc_verb_error("%s:%d:%s: asprintf", __FILE__, __LINE__, __func__);
	exit(EXIT_FAILURE);
    }
    rv = 0;
    nc_verb_verbose("%s: url: %s length: %d\nputdata: %s length: %d",
	    __func__, url, strlen(url), putdata, strlen(putdata));

    rv |= curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
    rv |= curl_easy_setopt(curl, CURLOPT_URL, url);
    rv |= curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, orca_write_callback);
    rv |= curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&resp);
    rv |= curl_easy_setopt(curl, CURLOPT_POSTFIELDS, putdata);
    rv |= curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long)strlen(putdata));

    if (rv != CURLE_OK) {
	nc_verb_error("%s:%d:%s: curl_east_setopt: %d",
		__FILE__, __LINE__, __func__, rv);
	goto cleanup;
    }

    rv = curl_easy_perform(curl);
    if (rv != CURLE_OK) {
	nc_verb_error("%s:%d:%s: curl_easy_perform: %d",
		__FILE__, __LINE__, __func__, rv);
	orca_resp = strdup("ERROR: curl_easy_perform");
	*status = (long)rv;
	goto cleanup;
    }
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
    nc_verb_verbose("Response: %ld", response_code);
    *status = response_code;

    /*
     * If there is a message element an application-level error has
     * occured and no data should be present in the agent reply.
     */
    if (strstr(resp.buffer, "message") != NULL) {
	orca_resp = get_reply_element(resp.buffer, "message");
	nc_verb_error("ERROR: %s", orca_resp);
	*status = -1;
	goto cleanup;
    }

    orca_resp = strdup(resp.buffer);
    nc_verb_verbose("Agent response: %s", orca_resp);

cleanup:
    free(resp.buffer);
    free(url);
    return orca_resp;
}

/**********************************************************************/

