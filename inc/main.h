#ifndef __CONFIG_H
#define __CONFIG_H
#include <stdint.h>

extern char *redis_host;
extern int redis_port;
extern char *capture_interface;
extern int *syn_expire;
extern int *ack_expire;
extern char *ipset_name;

int parse_args(int argc, char *argv[]);

int capture_start();

void mark_syn(uint32_t *ip);
void mark_ack(uint32_t *ip);
int redis_start();

void myipset_add(char *ip_str);
void myipset_del(char *ip_str);
int myipset_init();

#endif