#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>

char *redis_host;
int redis_port;
char *capture_interface;

static inline void copy_str(char **target, const char *from)
{
    *target = malloc(strlen(from) + 1);
    strcpy(*target, from);
}

int parse_args(int argc, char *argv[])
{
    int opt;
    while ((opt = getopt(argc, argv, "h:p:i:")) != -1)
    {
        switch (opt)
        {
        case 'h':
            copy_str(&redis_host, optarg);
            break;

        case 'p':
            redis_port = atoi(optarg);
            break;

        case 'i':
            copy_str(&capture_interface, optarg);
            break;
        }
    }
    if ((!redis_host) || (!redis_port) || (!capture_interface))
    {
        fprintf(stderr, "usage: %s -h rds_host -p rds_port -i eth_ifname\n", argv[0]);
        return 1;
    }
    printf("start options: redis=%s:%d, interface=%s\n", redis_host, redis_port, capture_interface);
    return 0;
}