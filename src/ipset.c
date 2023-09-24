#include <libipset/ipset.h>
#include "main.h"

static struct ipset *ipset;

void myipset_add(char *ip_str)
{
    char *line[4] = { NULL, "add", ipset_name, ip_str };
    int ret;
    ret = ipset_parse_argv(ipset, 4, line);
    if (ret < 0)
    {
        fprintf(stderr, "add ip [%s] to ipset [%s] failed\n", ip_str, ipset_name);
    }
}

void myipset_del(char *ip_str)
{
    char *line[4] = { NULL, "del", ipset_name, ip_str };
    ipset_parse_argv(ipset, 4, line);
}

static int cus_stderr(struct ipset *ipset, void *p)
{
}

int myipset_init()
{
    int ret;
    char *line[4] = { NULL, "flush", ipset_name, NULL };
    ipset_load_types();
    ipset = ipset_init();
    ipset_custom_printf(ipset, NULL, cus_stderr, NULL, NULL);

    ret = ipset_parse_argv(ipset, 3, line);
    if (ret < 0)
    {
        line[1] = "create";
        line[3] = "hash:ip";
        ret = ipset_parse_argv(ipset, 4, line);
        if (ret < 0)
        {
            fprintf(stderr, "failed to create ipset %s\n", ipset_name);
            return 1;
        }
        else
        {
            printf("new ipset %s created\n", ipset_name);
        }
    }
    else
    {
        printf("ipset %s flushed\n", ipset_name);
    }
    return 0;
}