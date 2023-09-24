#include <stdio.h>
#include "main.h"

int main(int argc, char *argv[])
{
    int ret;
    ret = parse_args(argc, argv);
    if (ret)
    {
        return ret;
    }
    ret = myipset_init();
    if (ret)
    {
        return ret;
    }
    ret = redis_start();
    if (ret)
    {
        return ret;
    }
    ret = capture_start();
    if (ret)
    {
        return ret;
    }
    return 0;
}