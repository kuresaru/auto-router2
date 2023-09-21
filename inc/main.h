#ifndef __CONFIG_H
#define __CONFIG_H

extern char *redis_host;
extern int redis_port;
extern char *capture_interface;

int parse_args(int argc, char *argv[]);

int capture_start();

#endif