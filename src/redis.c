#include <hiredis/hiredis.h>
#include <pthread.h>
#include <string.h>
#include <endian.h>
#include "main.h"

static redisContext *c;
static redisContext *sc;

void mark_syn(uint32_t *ip)
{
    redisReply *reply;
    int exists;

    reply = redisCommand(c, "EXISTS auto-router2:ack_%08x", *ip);
    exists = reply->integer;
    freeReplyObject(reply);

    if (!exists)
    {
        freeReplyObject(redisCommand(c, "SETEX auto-router2:syn_%08x %d 1", *ip, syn_expire));
    }
}

void mark_ack(uint32_t *ip)
{
    freeReplyObject(redisCommand(c, "DEL auto-router2:syn_%08x", *ip));
    freeReplyObject(redisCommand(c, "SETEX auto-router2:ack_%08x %d 1", *ip, ack_expire));
}

static void clear_keys()
{
    redisReply *reply, *item, *r;
    reply = redisCommand(c, "KEYS auto-router2:*");
    if (reply->type == REDIS_REPLY_ARRAY)
    {
        for (size_t i = 0; i < reply->elements; i++)
        {
            item = reply->element[i];
            if (item->type == REDIS_REPLY_STRING)
            {
                freeReplyObject(redisCommand(c, "DEL %s", item->str));
            }
        }
    }
    freeReplyObject(reply);
}

static inline void ip2str(uint32_t *ip, char *buf)
{
    uint8_t *p = (uint8_t *)ip;
#if __LITTLE_ENDIAN__
    sprintf(buf, "%d.%d.%d.%d", p[0], p[1], p[2], p[3]);
#else
    sprintf(buf, "%d.%d.%d.%d", p[3], p[2], p[1], p[0]);
#endif
}

static inline void expire_syn(uint32_t *ip)
{
    char ipbuf[16];
    ip2str(ip, ipbuf);
    printf("add %s syn expired\n", ipbuf);
    myipset_add(ipbuf);
}

static inline void expire_ack(uint32_t *ip)
{
    char ipbuf[16];
    ip2str(ip, ipbuf);
    printf("del %s ack expired\n", ipbuf);
    myipset_del(ipbuf);
}

static void *subscribe_thread(void *args)
{
    redisReply *reply, *key;
    uint32_t ip;
    char *end_ptr;
    printf("subscribe thread running\n");
    while (redisGetReply(sc, (void **)&reply) == REDIS_OK)
    {
        if ((reply->type == REDIS_REPLY_ARRAY) && (reply->elements == 4))
        {
            key = reply->element[3];
            if (key->type == REDIS_REPLY_STRING)
            {
                if (key->len == 25)
                {
                    if (!memcmp(key->str, "auto-router2:syn_", 17))
                    {
                        ip = strtoul(key->str + 17, &end_ptr, 16);
                        if (end_ptr == key->str + 25)
                        {
                            expire_syn(&ip);
                        }
                    }
                    else if (!memcmp(key->str, "auto-router2:ack_", 17))
                    {
                        ip = strtoul(key->str + 17, &end_ptr, 16);
                        if (end_ptr == key->str + 25)
                        {
                            expire_ack(&ip);
                        }
                    }
                }
            }
        }
        freeReplyObject(reply);
    }
    fprintf(stderr, "subscribe thread exit unexpected\n");
    exit(1);
    // pthread_exit(NULL);
}

int redis_start()
{
    pthread_t t;
    c = redisConnect(redis_host, redis_port);
    if (c->err)
    {
        fprintf(stderr, "failed to connect to redis server: %s\n", c->errstr);
        return 1;
    }
    sc = redisConnect(redis_host, redis_port);
    if (sc->err)
    {
        fprintf(stderr, "failed to connect to redis server: %s\n", sc->errstr);
        redisFree(c);
        return 1;
    }
    clear_keys();
    freeReplyObject(redisCommand(sc, "PSUBSCRIBE __keyevent@0__:expired"));
    if (pthread_create(&t, NULL, subscribe_thread, NULL) != 0)
    {
        fprintf(stderr, "failed to create redis subscribe thread.\n");
        redisFree(c);
        redisFree(sc);
        return 1;
    }
    return 0;
}