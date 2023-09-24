#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>

#define COPY_ARGS(a, d) case a: copy_str(&d, optarg); break;
#define COPY_ARGI(a, d) case a: d = atoi(optarg); break;

char *redis_host;
int redis_port;
char *capture_interface;
int syn_expire;
int ack_expire;
char *ipset_name;

static inline void copy_str(char **target, const char *from)
{
    *target = malloc(strlen(from) + 1);
    strcpy(*target, from);
}

int parse_args(int argc, char *argv[])
{
    int opt;
    while ((opt = getopt(argc, argv, "h:p:i:s:a:l:")) != -1)
    {
        switch (opt)
        {
            COPY_ARGS('h', redis_host)
            COPY_ARGI('p', redis_port)
            COPY_ARGS('i', capture_interface)
            COPY_ARGI('s', syn_expire)
            COPY_ARGI('a', ack_expire)
            COPY_ARGS('l', ipset_name)
        }
    }
    if ((!redis_host) || (!redis_port) || (!capture_interface) || (!syn_expire) ||
        (!ack_expire) || (!ipset_name))
    {
        fprintf(stderr, "usage: %s -h rds_host -p rds_port -i eth_ifname -s syn_expire -a ack_expire -l ipset_name\n", argv[0]);
        return 1;
    }
    printf("start options: redis=%s:%d, interface=%s, syn_expire=%d, ack_expire=%d, ipset_name=%s\n", 
        redis_host, redis_port, capture_interface, syn_expire, ack_expire, ipset_name);
    return 0;
}